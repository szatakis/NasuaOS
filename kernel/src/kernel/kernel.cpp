#include <cstdint>
#include <cstddef>
#include <limine.h>

#include "system/drivers/drivers.h"
#include "system/interrupts/interrupts.h"

#include "system/filesystem/ztrfs.h"

#include "applications/shell/commands.h"
#include "system/gui/vars/colors.h"
#include "system/gui/icons/icons.h"

#include "kernel/include/logger/logger.hpp"

#include "libs/libc/libc.h"
#include "libs/asm/asm.h"

__attribute__((used, section(".limine_requests")))
volatile limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST_ID,
    .revision = 0,
    .response = nullptr
};


__attribute__((used, section(".limine_requests")))
volatile limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0,
    .response = nullptr
};

__attribute__((used, section(".limine_requests")))
volatile limine_mp_request mp_request = {
    .id = LIMINE_MP_REQUEST_ID,
    .revision = 0,
    .response = nullptr,
    .flags = 0
};

// ---------------- LIMINE ----------------

namespace {
__attribute__((used, section(".limine_requests")))
volatile std::uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
volatile limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
    .response = nullptr
};

__attribute__((used, section(".limine_requests_start")))
volatile std::uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
volatile std::uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;
}

// ---------------- DEBUG VARS ----------------

bool debug_mode = false;

// ---------------- GLOBAL STATE ----------------

bool shift_pressed = false;
bool extended_scancode = false;

char command_buffer[64];
size_t cmd_idx = 0;

// ---------------- KEYBOARD HELPERS ----------------

extern char scancode_to_ascii_normal(uint8_t);
extern char scancode_to_ascii_shift(uint8_t);

// ---------------- KMAIN ----------------

extern "C" void kmain() {
    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
        hcf();
    }

    if (!framebuffer_request.response ||
        framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    fb = framebuffer_request.response->framebuffers[0];

    Uart::init();

    if(mp_request.response)
    {
        Uart::puts("[CPU] MP available\n");
        log(INFO,"CPU","MP available");
    }

    memory_init();
    paging_init();
    pmm_init();
    vmm_init();
    heap_init();
    apic_disable();
    pic_remap();
    idt_init();
    pit_init();

    asm volatile("sti");


    // Initialize backbuffer with framebuffer dimensions
    init_backbuffer(fb->width, fb->height, fb->pitch);

    clear_screen();
    update_bottom_bar();

    save_mouse_backdrop();
    draw_mouse_cursor();
    render_frame();

    fetch();
    print(CMD_TEXT_WHITE);
    print("Enter Command\n");
    print_cmd();

    for (;;) {
        updateTime();
        render_frame();

        uint8_t status = inb(0x64);

        if (!(status & 1))
            continue;

        uint8_t data = inb(0x60);

        // ---------------- KEYBOARD ----------------
        if (!(status & 0x20)) {
            uint8_t scancode = data;

            if(debug_mode) {
                print_sc(scancode);
            }

            // 1. Obsługa prefiksu rozszerzonego
            if (scancode == 0xE0) {
                extended_scancode = true;
                continue; // Czekamy na właściwy kod w następnej iteracji
            }

            // 2. Przetwarzanie kodów rozszerzonych (jeśli flaga była aktywna)
            if (extended_scancode) {
                extended_scancode = false; // Natychmiast czyścimy flagę, by nie zawisła

                // Sprawdzamy czy to puszczenie klawisza rozszerzonego (Break Code)
                if (scancode & 0x80) {
                    continue; // Ignorujemy puszczenie klawiszy rozszerzonych
                }

                // Left Windows (Wciśnięcie)
                if (scancode == 0x5B) {
                    if (!is_menu_start_open) {
                        clear_line();
                        print("Start menu opened\n");
                        print_cmd();
                        is_menu_start_open = true;
                    } else {
                        clear_line();
                        print("Start menu closed\n");
                        print_cmd();
                        is_menu_start_open = false;
                    }
                    continue;
                }

                // Right Windows (Wciśnięcie)
                if (scancode == 0x5C) {
                    if (!is_menu_start_open) {
                        clear_line();
                        print("Start menu opened\n");
                        print_cmd();
                        is_menu_start_open = true;
                    } else {
                        clear_line();
                        print("Start menu closed\n");
                        print_cmd();
                        is_menu_start_open = false;
                    }
                    continue;
                }

                // ARROW KEYS (Tylko rozszerzone wersje sterują myszą)
                if (scancode == 0x48 || scancode == 0x50 || scancode == 0x4B || scancode == 0x4D) {
                    restore_mouse_backdrop();
                    int speed = 5;

                    if (scancode == 0x48 && mouse_y >= speed)
                        mouse_y -= speed;
                    else if (scancode == 0x50 && mouse_y + 20 < (int)fb->height)
                        mouse_y += speed;
                    else if (scancode == 0x4B && mouse_x >= speed)
                        mouse_x -= speed;
                    else if (scancode == 0x4D && mouse_x + 20 < (int)fb->width)
                        mouse_x += speed;

                    save_mouse_backdrop();
                    draw_mouse_cursor();
                    render_frame();
                    continue;
                }

                continue; // Zabezpieczenie: pomiń resztę przetwarzania dla innych klawiszy E0
            }

            // 3. Normalne puszczenie klawisza (Zwykły Break Code bez E0)
            if (scancode & 0x80) {
                uint8_t rel = scancode & 0x7F;
                if (rel == 0x2A || rel == 0x36)
                    shift_pressed = false;
                continue;
            }

            // 4. SHIFT DOWN
            if (scancode == 0x2A || scancode == 0x36) {
                shift_pressed = true;
                continue;
            }

            // 5. BACKSPACE
            if (scancode == 0x0E) {
                if (cmd_idx > 0) {
                    cmd_idx--;
                    delete_last_char();
                    render_frame();
                }
                continue;
            }

            // 6. ENTER
            if (scancode == 0x1C) {
                if (is_mouse_over_start(mouse_x, mouse_y)) {
                    if (!is_menu_start_open) {
                        clear_line();
                        print("Start menu opened\n");
                        print_cmd();
                        is_menu_start_open = true;
                    } else {
                        clear_line();
                        print("Start menu closed\n");
                        print_cmd();
                        is_menu_start_open = false;
                    }
                } else {
                    command_buffer[cmd_idx] = '\0';
                    execute_command(command_buffer);
                    cmd_idx = 0;
                    render_frame();
                }
                continue;
            }

            // 7. ZWYKŁE ZNAKI (Tylko jeśli kod nie był rozszerzony)
            char c = shift_pressed
                ? scancode_to_ascii_shift(scancode)
                : scancode_to_ascii_normal(scancode);

            if (c && cmd_idx < 63) {
                command_buffer[cmd_idx++] = c;

                restore_mouse_backdrop();
                print_char8(c);
                save_mouse_backdrop();
                draw_mouse_cursor();
                render_frame();
            }
        }
    }
}

// ---------------- CRT ----------------

extern "C" {
int __cxa_atexit(void (*)(void*), void*, void*) { return 0; }
void __cxa_pure_virtual() { hcf(); }
void* __dso_handle;
}

extern void (*__init_array[])();
extern void (*__init_array_end[])();