#include "../driver.h"
#include "system/drivers/mouse/driver.h"

extern uint32_t mouse_backdrop[];

extern const char arrow_cursor[][CURSOR_W];

void save_mouse_backdrop() {
    if (!fb) return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    for (int y = 0; y < CURSOR_H; y++) {
        for (int x = 0; x < CURSOR_W; x++) {
            int px = mouse_x + x;
            int py = mouse_y + y;

            if (px < 0 || py < 0) continue;
            if ((size_t)px >= fb->width || (size_t)py >= fb->height) continue;

            mouse_backdrop[y * CURSOR_W + x] =
                bb_ptr[py * pitch + px];
        }
    }
}

void restore_mouse_backdrop() {
    if (!fb) return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    for (int y = 0; y < CURSOR_H; y++) {
        for (int x = 0; x < CURSOR_W; x++) {
            int px = mouse_x + x;
            int py = mouse_y + y;

            if (px < 0 || py < 0) continue;
            if ((size_t)px >= fb->width || (size_t)py >= fb->height) continue;

            bb_ptr[py * pitch + px] =
                mouse_backdrop[y * CURSOR_W + x];
        }
    }
}

void draw_mouse_cursor() {
    if (!fb) return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    for (int y = 0; y < CURSOR_H; y++) {
        for (int x = 0; x < CURSOR_W; x++) {
            int px = mouse_x + x;
            int py = mouse_y + y;

            if (px < 0 || py < 0) continue;
            if ((size_t)px >= fb->width || (size_t)py >= fb->height) continue;

            char p = arrow_cursor[y][x];

            if (p == 'W')
                bb_ptr[py * pitch + px] = 0xFFFFFFFF;
            else if (p == 'B')
                bb_ptr[py * pitch + px] = 0x00000000;
        }
    }
}