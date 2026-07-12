#pragma once
#include <cstddef>
#include <cstdint>

constexpr size_t ICON_W = 32;
constexpr size_t ICON_H = 32;

extern const char start_icon[ICON_H][ICON_W];
extern const char start_icon_hover[ICON_H + 2][ICON_W + 2];

extern bool is_menu_start_open;
extern bool start_hover;

template <size_t rows, size_t cols>
void draw_icon(const char (&icon)[rows][cols], size_t start_x, size_t start_y);
void draw_start_button(size_t x, size_t y);
bool is_mouse_over_start(int mouse_x, int mouse_y);