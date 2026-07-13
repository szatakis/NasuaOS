#pragma once
#include <cstddef>
#include <cstdint>

#include "system/drivers/video/driver.h"
#include "../vars/colors.h"

extern const uint32_t notification_icon[1024];
extern const uint32_t notification_icon_h[1024];
extern const uint32_t speaker_icon[1024];


extern bool is_menu_start_open;
extern bool start_hover;

void draw_start_button(size_t x, size_t y);
bool is_mouse_over_start(int mouse_x, int mouse_y);

void draw_start_menu_system_icons(int x, int y);

template <size_t rows, size_t cols>
void draw_icon(const char (&icon)[rows][cols], size_t start_x, size_t start_y) {
    if (!fb) return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    for (size_t y = 0; y < rows; y++) {
        for (size_t x = 0; x < cols; x++) {

            size_t px = start_x + x;
            size_t py = start_y + y;

            if (px >= fb->width || py >= fb->height)
                continue;

            char p = icon[y][x];

            uint32_t color;
            bool draw = true;

            switch (p) {
                case 'W': color = 0xFFFFFFFF; break;
                case 'K': color = 0xFF000000; break;
                case 'E': color = 0xFF424240; break;
                case 'B': color = 0xFF213BBF; break;
                case 'R': color = 0xFFDE1D16; break;
                case 'G': color = 0xFF22B31D; break;
                case 'Y': color = 0xFFE6CD10; break;
                case 'x': color = 0xFF808080; break;
                default:
                    draw = false;
                    break;
            }

            if(draw)
                bb_ptr[py * pitch + px] = color;
        }
    }
}

template <size_t width, size_t height>
void draw_icon_hex(const uint32_t* icon, size_t start_x, size_t start_y)
{
    if (!fb) return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            size_t px = start_x + x;
            size_t py = start_y + y;

            if (px >= fb->width || py >= fb->height)
                continue;

            uint32_t color = icon[y * width + x];

            if (color == 0x000000)
                continue;

            bb_ptr[py * pitch + px] = 0xFF000000 | color;
        }
    }
}