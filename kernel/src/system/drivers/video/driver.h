#pragma once
#include <cstddef>
#include <cstdint>
#include <limine.h>

extern limine_framebuffer* fb;

extern size_t cursor_x;
extern size_t cursor_y;

extern uint32_t current_text_color;

extern const uint32_t FONT_SPACING_W;
extern const uint32_t FONT_SPACING_H;

// Backbuffer management
void init_backbuffer(size_t width, size_t height, size_t pitch);
uint32_t* get_backbuffer();
size_t get_backbuffer_pitch();
void render_frame();  // Copy entire backbuffer to framebuffer

void clear_screen();
void clear_line();

void print(const char* str);
void print_int(int value);
void print_char8(char c);

void print_at8(const char* str, size_t x, size_t y, uint32_t color);
void print_at10(const char* str, size_t x, size_t y, uint32_t color);
void print_at12(const char* str, size_t x, size_t y, uint32_t color);
void print_at16(const char* str, size_t x, size_t y, uint32_t color);

void delete_last_char();

void save_mouse_backdrop();
void restore_mouse_backdrop();
void draw_mouse_cursor();

// ---------------- DRAW API ----------------
void update_bottom_bar();

// ---------------- FONT DRAW ----------------
void draw_char8(unsigned char c, size_t x, size_t y, uint32_t color);
void draw_char10(unsigned char c, size_t x, size_t y, uint32_t color);
void draw_char12(unsigned char c, size_t x, size_t y, uint32_t color);
void draw_char16(unsigned char c, size_t x, size_t y, uint32_t color);

void print_num8(uint32_t num);

void updateTime();
void draw_rect(int x1, int y1, int x2, int y2, uint32_t color);

void print_info(const char* msg);
void print_warn(const char* msg);
void print_error(const char* msg);
void print_cmd();

void print_num_padded(uint32_t num);
void print_year(uint32_t year);

void print_resolution();

void fetch();


//Window manager
struct window_struct {
    const char* name;
    uint32_t id;

    int pos_x;
    int pos_y;
    int width;
    int height;

    bool visible;
    bool focused;
};

void draw_window(window_struct* window);