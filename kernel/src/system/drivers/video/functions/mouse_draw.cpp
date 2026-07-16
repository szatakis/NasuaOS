#include "../driver.h"
#include "system/drivers/mouse/driver.h"
#include "system/gui/vars/colors.h"

extern const char arrow_cursor[][CURSOR_W];

// Pobiera kształt z arrow_cursor i renderuje go bezpośrednio do backbuffera
void handle_mouse() 
{
    if (!fb) return;

    uint32_t* bb_ptr = get_backbuffer();
    size_t bb_pitch = get_backbuffer_pitch();

    for (int y = 0; y < CURSOR_H; y++) 
    {
        for (int x = 0; x < CURSOR_W; x++) 
        {
            int px = mouse_x + x;
            int py = mouse_y + y;

            // Sprawdzenie granic ekranu
            if (px < 0 || py < 0) continue;
            if ((size_t)px >= fb->width || (size_t)py >= fb->height) continue;

            char p = arrow_cursor[y][x];
            uint32_t color = 0;

            // Logika wyboru koloru / przezroczystości
            if (p == 'W') 
            {
                color = COLOR_MOUSE;
            } 
            else if (p == 'B') 
            {
                color = 0x00000000; // Czarny / krawędź
            } 
            else 
            {
                continue; // Piksel przezroczysty - pomiń i nie nadpisuj backbuffera
            }

            // Zapis bezpośrednio do BACKBUFFERA
            bb_ptr[py * bb_pitch + px] = color;
        }
    }
}
