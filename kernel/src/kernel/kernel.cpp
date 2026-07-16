#include <cstdint>
#include <cstddef>
#include <limine.h>

#include "system/drivers/drivers.h"
#include "system/interrupts/interrupts.h"
#include "system/filesystem/clawfs.h"

#include "applications/shell/commands.h"

#include "system/gui/vars/colors.h"
#include "system/gui/icons/icons.h"
#include "system/gui/gui.h"

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
    volatile limine_framebuffer_request framebuffer_request = 
    {
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

// ---------------- KMAIN ----------------

extern "C" void kmain() 
{
    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) 
    {
        hcf();
    }

    if (!framebuffer_request.response || framebuffer_request.response->framebuffer_count < 1) 
    {
        hcf();
    }

    fb = framebuffer_request.response->framebuffers[0];
    // Initialize backbuffer with framebuffer dimensions
    init_backbuffer(fb->width, fb->height, fb->pitch);
    init_text_buffer();

    Uart::init();

    if(mp_request.response)
    {
        Uart::puts("[CPU] MP available\n");
        log(INFO,"CPU","MP available");
    }

    storage_init();
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

    // Główna pętla wywołań
    for (;;) 
    {
        clear_screen();

        update_gui();
        update_windows_gui();

        handle_keyboard();
        handle_mouse();

        render_frame();

        asm volatile("hlt");
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