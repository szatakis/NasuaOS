#include "icons.h"

bool start_hover = false;

int icons_start_x = 0;
int icons_start_y = 0;
int icons_offset = 45;

typedef struct 
{
    int x1, y1;
    int x2, y2;
} ButtonArea;

int bar_h = 36; 
int start_y = static_cast<int>(fb->height) - bar_h;  
bool is_menu_start_open = false;

ButtonArea start_button = { 0, 0, 0, 0 }; 

void draw_start_button(size_t x, size_t y) 
{
    start_button.x1 = static_cast<int>(x);
    start_button.y1 = static_cast<int>(y);
    start_button.x2 = static_cast<int>(x + 100);
    start_button.y2 = static_cast<int>(y + 31); 

    int margin = 2;

    draw_rect(start_button.x1 - margin, start_button.y1, start_button.x2 + margin, start_button.y2, COLOR_NASUA_DARK);

    draw_rect(start_button.x1, start_button.y1 + margin, start_button.x2, start_button.y2 - margin, COLOR_NASUA_DARKER);

    if(start_hover)
    {
        draw_icon<32, 32>(start_icon_hover, x, y);
    } 
    else 
    {
        draw_icon<32, 32>(start_icon, x, y);
    }
    
    print_at10("START", x + 32 + 6, y + 12, COLOR_WHITE);
}

void draw_start_menu_system_icons(int x, int y, int p, int w, int h) 
{
    (void)w;
    draw_icon<32, 32>(shutdown_icon, (x + (p - 32) / 2), (y + h - (32 * 2) - 16));
    draw_icon<32, 32>(reboot_icon, (x + (p - 32) / 2), (y + h - (32 * 2) + 24));

    icons_start_x = x;
    icons_start_y = y + 5;

    draw_icon<32, 32>(terminal_icon, icons_start_x + 50, icons_start_y);
    print_at8("Terminal", icons_start_x + 85, icons_start_y + 12, COLOR_WHITE);

    draw_icon<32, 32>(settings_icon, icons_start_x + 50, icons_start_y + icons_offset);
    print_at8("Settings", icons_start_x + 85, icons_start_y + 12 + icons_offset, COLOR_WHITE); // not work yet

    draw_icon<32, 32>(suaedit_icon, icons_start_x + 50, icons_start_y + 2 * icons_offset);
    print_at8("SuaEdit", icons_start_x + 85, icons_start_y + 12 + 2 * icons_offset, COLOR_WHITE); // not work yet

    draw_icon<32, 32>(calculator_icon, icons_start_x + 50, icons_start_y + 3 * icons_offset);
    print_at8("Calculator", icons_start_x + 85, icons_start_y + 12 + 3 * icons_offset, COLOR_WHITE);
}

bool is_mouse_over_start(int mouse_x, int mouse_y) 
{
    if (mouse_x >= start_button.x1 && mouse_x <= start_button.x2 && mouse_y >= start_button.y1 && mouse_y <= start_button.y2) 
    {
        return true;
    }
    return false;
}

bool is_mouse_over_icon(int mx, int my, int x, int y, int w, int h)
{
    return mx >= x &&
           mx < x + w &&
           my >= y &&
           my < y + h;
}