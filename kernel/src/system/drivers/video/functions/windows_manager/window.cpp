#include "../../driver.h"
#include "window.h"

#include "system/drivers/video/driver.h"
#include "system/gui/vars/colors.h"

void draw_window(window_struct* window) {
    if (!window || !window->visible)
        return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    for (int y = 0; y < window->height; y++) {
        for (int x = 0; x < window->width; x++) {
            bb_ptr[(window->pos_y + y) * pitch + (window->pos_x + x)] = COLOR_BLACK;
        }
    }
    for (int y = 0; y < (window->height/10); y++) {
        for (int x = 0; x < window->width; x++) {
            bb_ptr[(window->pos_y + y) * pitch + (window->pos_x + x)] = COLOR_DARK_GRAY;
        }
    }

    print_at8(window->name, (window->pos_x + 10), (window->pos_y + ((((window->height) / 10) - 8) / 2)), COLOR_WHITE);

    int button_x = window->pos_x + window->width - 55;
    int button_y = window->pos_y + ((window->height / 10) - 8) / 2;

    print_at8("-", button_x, button_y, COLOR_WHITE);
    print_at8("[]", button_x + 16, button_y, COLOR_WHITE);
    print_at8("X", button_x + 42, button_y, COLOR_WHITE);
}