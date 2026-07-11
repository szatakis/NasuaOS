#include "../../driver.h"
#include "system/gui/vars/colors.h"
#include "applications/shell/commands.h"

limine_framebuffer* fb = nullptr;

// Backbuffer: static allocation for double buffering
// Maximum resolution support: 4096x2160 @ 32bpp = ~33 MB
static uint32_t backbuffer[4096 * 2160];
static size_t backbuffer_width = 0;
static size_t backbuffer_height = 0;
static size_t backbuffer_pitch = 0;

void init_backbuffer(size_t width, size_t height, size_t pitch) {
    backbuffer_width = width;
    backbuffer_height = height;
    backbuffer_pitch = pitch / 4;  // Convert to pixel pitch
}

uint32_t* get_backbuffer() {
    return backbuffer;
}

size_t get_backbuffer_pitch() {
    return backbuffer_pitch;
}

void render_frame() {
    if (!fb) return;

    uint32_t* fb_ptr = (uint32_t*)fb->address;
    
    // Copy entire backbuffer to framebuffer
    for (size_t y = 0; y < backbuffer_height; y++) {
        for (size_t x = 0; x < backbuffer_width; x++) {
            fb_ptr[y * backbuffer_pitch + x] = backbuffer[y * backbuffer_pitch + x];
        }
    }
}

void clear_screen() {
    if (!fb) return;

    cursor_x = 0;
    cursor_y = 0;

    uint32_t* bb_ptr = backbuffer;

    size_t bar_height = 36;
    size_t bar_start = backbuffer_height - bar_height;

    for (size_t y = 0; y < backbuffer_height; y++) {
        uint32_t c = (y >= bar_start) ? COLOR_DARK_GRAY : COLOR_ZYTR_BG;

        for (size_t x = 0; x < backbuffer_width; x++) {
            bb_ptr[y * backbuffer_pitch + x] = c;
        }
    }
}