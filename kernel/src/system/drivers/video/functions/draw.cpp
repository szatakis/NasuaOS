#include "../driver.h"

#include "system/fonts/font8x8.h"
#include "system/fonts/font10x10.h"
#include "system/fonts/font12x12.h"
#include "system/fonts/font16x16.h"

#include "system/gui/icons/icons.h"
#include "system/drivers/rtc/driver.h"
#include "system/drivers/mouse/driver.h"

#include "system/gui/vars/colors.h"
#include "system/gui/gui.h"

#include "system/sysfunc/notification_manager/notification_manager.h"

// ---------------- HELPERS ----------------

static inline void put_pixel(size_t x, size_t y, uint32_t color) 
{
    if (!fb) return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    if (x >= fb->width || y >= fb->height) return;

    bb_ptr[y * pitch + x] = color;
}

// ---------------- FONT 8x8 ----------------
void draw_char8(unsigned char c, size_t x, size_t y, uint32_t color) 
{
    for (int row = 0; row < 8; row++) 
    {
        uint8_t bits = font8x8[c][row];

        for (int col = 0; col < 8; col++) 
        {
            if (bits & (1 << (7 - col))) 
            {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

// ---------------- FONT 10x10 ----------------
void draw_char10(unsigned char c, size_t x, size_t y, uint32_t color) 
{
    for (int row = 0; row < 10; row++) 
    {
        uint16_t bits = font10x10[c][row];

        for (int col = 0; col < 10; col++) 
        {
            if (bits & (1 << (9 - col))) 
            {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

// ---------------- FONT 12x12 ----------------
void draw_char12(unsigned char c, size_t x, size_t y, uint32_t color) 
{
    for (int row = 0; row < 12; row++) 
    {
        uint16_t bits = font12x12[c][row];

        for (int col = 0; col < 12; col++) 
        {
            if (bits & (1 << (11 - col))) 
            {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

// ---------------- FONT 16x16 ----------------
void draw_char16(unsigned char c, size_t x, size_t y, uint32_t color) 
{
    for (int row = 0; row < 16; row++) 
    {
        uint16_t bits = font16x16[c][row];

        for (int col = 0; col < 16; col++) 
        {
            if (bits & (1 << (15 - col))) 
            {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

void fill_block(size_t x, size_t y, uint32_t color, size_t size_x, size_t size_y) 
{
    if (!fb) return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t pitch = get_backbuffer_pitch();

    // Clipping - zabezpieczenie przed wyjściem poza ekran
    if (x >= fb->width || y >= fb->height) return;
    if (x + size_x > fb->width)  size_x = fb->width - x;
    if (y + size_y > fb->height) size_y = fb->height - y;

    for (size_t dy = 0; dy < size_y; dy++) 
    {
        for (size_t dx = 0; dx < size_x; dx++) 
        {
            bb_ptr[(y + dy) * pitch + (x + dx)] = color;
        }
    }
}

void update_start() 
{
    if (!fb) return;
    size_t bar_h = 36;
    size_t start_y = fb->height - bar_h;

    // Usunięto fill_block czyszczący, ponieważ update_bottom_bar robi to globalnie
    draw_start_button(5, start_y + 2);
}

void update_time() 
{
    if (!fb) return;

    RtcTime time = get_rtc_time();

    char time_buf[9];
    char date_buf[11];

    // ---------------- TIME HH:MM:SS ----------------
    time_buf[0] = '0' + (time.hour / 10);
    time_buf[1] = '0' + (time.hour % 10);
    time_buf[2] = ':';
    time_buf[3] = '0' + (time.minute / 10);
    time_buf[4] = '0' + (time.minute % 10);
    time_buf[5] = ':';
    time_buf[6] = '0' + (time.second / 10);
    time_buf[7] = '0' + (time.second % 10);
    time_buf[8] = '\0';

    // ---------------- DATE DD-MM-YYYY ----------------
    date_buf[0] = '0' + (time.day / 10);
    date_buf[1] = '0' + (time.day % 10);
    date_buf[2] = '-';
    date_buf[3] = '0' + (time.month / 10);
    date_buf[4] = '0' + (time.month % 10);
    date_buf[5] = '-';

    date_buf[6] = '0' + (time.year / 1000);
    date_buf[7] = '0' + ((time.year / 100) % 10);
    date_buf[8] = '0' + ((time.year / 10) % 10);
    date_buf[9] = '0' + (time.year % 10);
    date_buf[10] = '\0';

    size_t text_x = fb->width - 105;
    size_t bar_h = 36;
    size_t start_y = fb->height - bar_h;

    // Usunięto zbędne wywołania fill_block do czyszczenia tła pod tekstem zegarka,
    // ponieważ cała linia paska jest świeżo narysowana w update_bottom_bar()

    // ---------------- DRAW ----------------
    print_at8(time_buf, text_x, start_y + 8, COLOR_WHITE);
    print_at8(date_buf, text_x, start_y + 20, COLOR_WHITE);
}

void draw_rect(int x1, int y1, int x2, int y2, uint32_t color) 
{
    if (!fb) return;

    // 1. Zabezpieczenie współrzędnych
    if (x1 > x2) 
    { 
        int tmp = x1; x1 = x2; x2 = tmp; 
    }
    if (y1 > y2) 
    { 
        int tmp = y1; y1 = y2; y2 = tmp; 
    }

    // 2. Clipping (ochrona przed wyjściem poza ekran)
    if (x1 < 0) 
    {
        x1 = 0;
    }
    if (y1 < 0) 
    {
        y1 = 0;
    }
    if (x2 >= static_cast<int>(fb->width))  
    {
        x2 = static_cast<int>(fb->width) - 1;
    }
    if (y2 >= static_cast<int>(fb->height)) 
    {
        y2 = static_cast<int>(fb->height) - 1;
    }

    // 3. Pobranie wskaźnika na backbuffer
    uint32_t* bb_ptr = get_backbuffer();
    int pixels_per_pitch = get_backbuffer_pitch();

    // 4. Rysowanie prostokąta
    for (int y = y1; y < y2; ++y) 
    {
        for (int x = x1; x < x2; ++x) 
        {
            bb_ptr[y * pixels_per_pitch + x] = color;
        }
    }
}

void update_bottom_bar() 
{
    if (!fb) return;

    size_t bar_h = 36;
    size_t start_y = fb->height - bar_h;
    size_t start_x = fb->width;

    // Rysujemy podkład paska zadań
    fill_block(0, start_y, COLOR_NASUA_TASKBAR, fb->width, bar_h);

    // Nakładamy ikony po prawej stronie
    draw_icon<32, 32>(speaker_icon, start_x - 175, start_y + ((bar_h - 32) / 2));
    if(notification_count <= 0) 
    {
        draw_icon<32, 32>(notification_icon, start_x - 145, start_y + ((bar_h - 32) / 2));
    }
    else 
    {
        draw_icon<32, 32>(notification_icon_h, start_x - 145, start_y + ((bar_h - 32) / 2));
    }

    // Rysujemy przycisk START i czas
    update_start();
    update_time();
}

void update_gui() 
{
    update_gui_state(mouse_x, mouse_y);

    update_bottom_bar();
    draw_text_buffer();
    draw_start_menu();
}
