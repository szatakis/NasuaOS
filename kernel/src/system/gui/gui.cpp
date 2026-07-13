#include "./icons/icons.h"
#include "system/drivers/video/driver.h"
#include "system/drivers/memory/driver.h"
#include "system/drivers/keyboard/driver.h"
#include "system/drivers/mouse/driver.h"
#include "./vars/colors.h"

extern limine_framebuffer* fb;

bool last_start_hover = false;

const uint32_t menu_w = 300;
const uint32_t menu_h = 400;
uint32_t menu_x = 0;
uint32_t menu_y = 0;

// Zewnętrzne deklaracje wymaganych funkcji i zmiennych (zależnie od reszty Twojego OS)
extern bool start_hover;
extern bool is_menu_start_open;
extern void draw_rect(int x1, int y1, int x2, int y2, uint32_t color);
extern bool is_mouse_over_start(int mouse_x, int mouse_y);
extern void draw_start_menu_system_icons(int x, int y);

void update_gui_state(int mouse_x, int mouse_y) {
    // Sprawdzamy czy mysz jest nad przyciskiem i aktualizujemy flagę
    if (is_mouse_over_start(mouse_x, mouse_y)) {
        start_hover = true;
    } else {
        start_hover = false;
    }
}

// Funkcja rysująca menu (przyjmuje bezwzględne współrzędne ekranu jako X i Y oraz Szerokość i Wysokość)
void draw_start_menu_f(int x, int y, int w, int h) {
    int panel_width = 45;

    // Poprawiono matematykę draw_rect: draw_rect oczekuje współrzędnych (x1, y1, x2, y2)
    
    // Prawy główny panel menu Start
    draw_rect(x, y, x + w, y + h, COLOR_NASUA_START_MENU);
    // Lewy panel boczny menu Start
    draw_rect(x, y, x + panel_width, y + h, COLOR_NASUA_START_MENU_P);

    // Rysowanie ikon systemowych na panelu bocznym
    draw_start_menu_system_icons((x + (panel_width - 32) / 2), (y + h - (32 * 2) - 16));
}

// NOWA FUNKCJA: Wywołaj ją w każdej klatce pętli głównej systemu!
void draw_start_menu() {
    if (!fb || !is_menu_start_open) return;

    // Aktualizacja pozycji (w razie zmiany rozdzielczości ekranu)
    menu_x = 0;
    menu_y = fb->height - 36 - menu_h; // 36 to wysokość paska zadań

    draw_start_menu_f(menu_x, menu_y, menu_w, menu_h);
}

void open_start_menu() {
    if (!fb) return;
    // W nowym systemie ta funkcja jedynie aktywuje flagę logiczną otwarcia menu
    is_menu_start_open = true;
}

void close_start_menu() {
    is_menu_start_open = false;
}
