#include "../driver.h"
#include "system/fonts/font8x8.h"
#include "system/fonts/font10x10.h"
#include "system/fonts/font12x12.h"
#include "system/fonts/font16x16.h"
#include "system/gui/vars/colors.h"
#include "applications/shell/commands.h"
#include "libs/libc/libc.h"

uint32_t current_text_color = COLOR_WHITE;

size_t cursor_x = 0;
size_t cursor_y = 0;

const uint32_t FONT_SPACING_W = 1;
const uint32_t FONT_SPACING_H = 4;

extern char current_path[256];
extern char user_name[16];
extern char pc_name[16];

void print_char8(char c) {
    size_t bar = fb->height - 45;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 8 + FONT_SPACING_H;

        if (cursor_y > bar) {
            cursor_x = 0;
            cursor_y = 0;
            clear_screen();
            update_bottom_bar();
            draw_mouse_cursor();
            fetch();
            print(CMD_TEXT_YELLOW);
            print("Enter Command\n");
            print(CMD_TEXT_WHITE);
        }
        return;
    }

    draw_char8(c, cursor_x, cursor_y, current_text_color);

    cursor_x += 8 + FONT_SPACING_W;

    if (cursor_x + 8 >= fb->width)
        print_char8('\n');
}

void print_num8(uint32_t num)
{
    char buffer[12];
    int i = 0;

    if(num == 0)
    {
        print("0");
        return;
    }

    while(num > 0)
    {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while(i > 0)
    {
        print_char8(buffer[--i]);
    }
}

void print(const char* str) {
    while (*str) {
        if (*str == '\x1B' && *(str + 1) == '[') {

            char code = *(str + 2);

            switch (code) {
                case 'w': current_text_color = COLOR_WHITE; break;
                case 'r': current_text_color = COLOR_RED; break;
                case 'g': current_text_color = COLOR_GREEN; break;
                case 'b': current_text_color = COLOR_BLUE; break;
                case 'y': current_text_color = COLOR_YELLOW; break;
                case 'a': current_text_color = COLOR_GRAY; break;
            }

            str += 3;
            continue;
        }

        print_char8(*str++);
    }
}

void print_int(int value)
{
    char buffer[32];
    int i = 0;

    if (value == 0)
    {
        print("0");
        return;
    }

    if (value < 0)
    {
        print("-");
        value = -value;
    }

    while (value > 0)
    {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    // odwracanie znaków
    for (int j = i - 1; j >= 0; j--)
    {
        char c[2];
        c[0] = buffer[j];
        c[1] = '\0';

        print(c);
    }
}

void print_at8(const char* str, size_t x, size_t y, uint32_t color)
{
    while (*str) {
        if (*str == '\n') {
            x = 0;
            y += 12;
            str++;
            continue;
        }

        draw_char8(*str, x, y, color);
        x += 8 + FONT_SPACING_W;
        str++;
    }
}

void print_at10(const char* str, size_t x, size_t y, uint32_t color)
{
    while (*str) {
        if (*str == '\n') {
            x = 0;
            y += 14;
            str++;
            continue;
        }

        draw_char10(*str, x, y, color);
        x += 10 + FONT_SPACING_W;
        str++;
    }
}

void print_at12(const char* str, size_t x, size_t y, uint32_t color)
{
    while (*str) {
        if (*str == '\n') {
            x = 0;
            y += 16;
            str++;
            continue;
        }

        draw_char12(*str, x, y, color);
        x += 12 + FONT_SPACING_W;
        str++;
    }
}

void print_at16(const char* str, size_t x, size_t y, uint32_t color)
{
    while (*str) {
        if (*str == '\n') {
            x = 0;
            y += 20;
            str++;
            continue;
        }

        draw_char16(*str, x, y, color);
        x += 16 + FONT_SPACING_W;
        str++;
    }
}

void delete_last_char() {
    if (cursor_x >= 8 + FONT_SPACING_W)
        cursor_x -= 8 + FONT_SPACING_W;
    else if (cursor_y >= 12) {
        cursor_y -= 12;
        cursor_x = fb->width - 8 + FONT_SPACING_W;
    } 
    else {
        return;
    }

    if (!fb)
        return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    size_t x = cursor_x;
    size_t y = cursor_y;

    uint32_t bg = COLOR_ZYTR_BG;

    for (int dy = 0; dy < 8; dy++) {
        for (uint32_t dx = 0; dx < 8 + FONT_SPACING_W; dx++) {
            bb_ptr[(y + dy) * pitch + (x + dx)] = bg;
        }
    }
}

void clear_line() {
    if (!fb) return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    uint32_t bg = COLOR_ZYTR_BG;

    size_t start_x = cursor_x - (cmd_idx * (8 + FONT_SPACING_W));

    for (size_t x = start_x; x < cursor_x; x++) {
        for (int y = 0; y < 8; y++) {
            bb_ptr[(cursor_y + y) * pitch + x] = bg;
        }
    }

    cursor_x = start_x;

    for (size_t i = 0; i < sizeof(command_buffer); i++) {
        command_buffer[i] = '\0';
    }

    cmd_idx = 0;
}

void print_info(const char* msg) {
    print(CMD_TEXT_GREEN);
    print("[info]: ");
    print(CMD_TEXT_WHITE);
    print(msg);
}

void print_warn(const char* msg) {
    print(CMD_TEXT_YELLOW);
    print("[warning]: ");
    print(CMD_TEXT_WHITE);
    print(msg);
}

void print_error(const char* msg) {
    print(CMD_TEXT_RED);
    print("[error]: ");
    print(CMD_TEXT_WHITE);
    print(msg);
}

void fetch() {
    print(CMD_TEXT_WHITE);
    print("\n  $$$$$$$$\\            $$\\                $$$$$$\\   $$$$$$\\  \n");
    print("  \\____$$  |           $$ |              $$  __$$\\ $$  __$$\\ \n");
    print("      $$  /$$\\   $$\\ $$$$$$\\    $$$$$$\\  $$ /  $$ |$$ /  \\__|\n");
    print("     $$  / $$ |  $$ |\\_$$  _|  $$  __$$\\ $$ |  $$ |\\$$$$$$\\\n");
    print("    $$  /  $$ |  $$ |  $$ |    $$ |  \\__|$$ |  $$ | \\____$$\\ \n");
    print("   $$  /   $$ |  $$ |  $$ |$$\\ $$ |      $$ |  $$ |$$\\   $$ |\n");
    print("  $$$$$$$$\\\\$$$$$$$ |  \\$$$$  |$$ |       $$$$$$  |\\$$$$$$  |\n");
    print("  \\________|\\____$$ |   \\____/ \\__|       \\______/  \\______/ \n");
    print("           $$\\   $$ |                                        \n");
    print("           \\$$$$$$  |                                        \n");
    print("            \\______/                                         \n\n\n");
}

void print_cmd() {
    print(CMD_TEXT_BLUE);
    print(user_name);
    print("@");
    print(pc_name);
    print(CMD_TEXT_WHITE); 
    print(":");
    print(CMD_TEXT_YELLOW);
    print(current_path);
    print(CMD_TEXT_GRAY);
    print("> ");
    print(CMD_TEXT_WHITE); 
}

// Funkcja pomocnicza zamieniająca liczbę na tekst z zerem wiodącym (np. 5 -> "05")
void print_num_padded(uint32_t num) {
    char buf[3];
    buf[0] = '0' + (num / 10);
    buf[1] = '0' + (num % 10);
    buf[2] = '\0';
    print(buf); // Wywołanie Twojej oryginalnej funkcji print
}

// Przeciążenie dla roku (obsługuje 4 cyfry)
void print_year(uint32_t year) {
    char buf[5];
    buf[0] = '0' + (year / 1000);
    buf[1] = '0' + ((year % 1000) / 100);
    buf[2] = '0' + ((year % 100) / 10);
    buf[3] = '0' + (year % 10);
    buf[4] = '\0';
    print(buf);
}

void print_resolution()
{
    if (!fb)
    {
        print("Framebuffer not available\n");
        return;
    }

    print_info("Screen resolution: ");
    print_int(fb->width);
    print("x");
    print_int(fb->height);
    print("\n");
}