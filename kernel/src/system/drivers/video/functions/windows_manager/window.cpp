#include "../../driver.h"
#include "window.h"

#include "system/drivers/video/driver.h"
#include "system/gui/vars/colors.h"

#define MAX_WINDOWS 10

// Globalna tablica przechowująca wskaźniki do okien w systemie
static window_struct* window_list[MAX_WINDOWS];
static int window_count = 0;

// Rejestruje okno w systemie, aby pętla główna wiedziała, że ma je stale rysować
void register_window(window_struct* window) {
    if (window_count >= MAX_WINDOWS) return;
    
    // Sprawdzamy, czy okno nie jest już zarejestrowane
    for (int i = 0; i < window_count; i++) {
        if (window_list[i] == window) return;
    }
    
    window_list[window_count++] = window;
}

// Wyrejestrowuje okno (np. przy zamknięciu 'X')
void unregister_window(window_struct* window) {
    for (int i = 0; i < window_count; i++) {
        if (window_list[i] == window) {
            // Przesuwamy resztę okien w tablicy
            for (int j = i; j < window_count - 1; j++) {
                window_list[j] = window_list[j + 1];
            }
            window_count--;
            return;
        }
    }
}

// Rysuje wszystkie zarejestrowane okna (WYWOŁAJ TO CO KLATKĘ)
void draw_windows() {
    for (int i = 0; i < window_count; i++) {
        if (window_list[i]->visible) {
            draw_window(window_list[i]);
        }
    }
}

void draw_window(window_struct* window) {
    if (!window || !window->visible || !fb)
        return;

    // Zabezpieczenie: jeśli okno jest całkowicie poza ekranem, nie rysuj
    if (window->pos_x >= (int)fb->width || window->pos_y >= (int)fb->height) return;
    if (window->pos_x + window->width <= 0 || window->pos_y + window->height <= 0) return;

    int title_height = window->height / 10;
    // Pasek tytułowy powinien mieć jakąś minimalną rozsądną wysokość (np. 16-20px)
    if (title_height < 18) title_height = 18; 

    // 1. Rysowanie głównego ciała okna (używamy bezpiecznego fill_block z clippingiem)
    if (window->pos_x >= 0 && window->pos_y >= 0) {
        fill_block(window->pos_x, window->pos_y, COLOR_WINDOW, window->width, window->height);
    } else {
        // Obsługa sytuacji, gdy okno jest częściowo wysunięte za lewą lub górną krawędź
        int draw_x = window->pos_x < 0 ? 0 : window->pos_x;
        int draw_y = window->pos_y < 0 ? 0 : window->pos_y;
        int draw_w = window->pos_x < 0 ? window->width + window->pos_x : window->width;
        int draw_h = window->pos_y < 0 ? window->height + window->pos_y : window->height;
        if (draw_w > 0 && draw_h > 0) {
            fill_block(draw_x, draw_y, COLOR_WINDOW, draw_w, draw_h);
        }
    }

    // 2. Rysowanie paska tytułowego (na wierzchu ciała okna)
    int title_draw_x = window->pos_x < 0 ? 0 : window->pos_x;
    int title_draw_y = window->pos_y < 0 ? 0 : window->pos_y;
    int title_draw_w = window->pos_x < 0 ? window->width + window->pos_x : window->width;
    int title_draw_h = window->pos_y < 0 ? title_height + window->pos_y : title_height;
    if (title_draw_w > 0 && title_draw_h > 0) {
        fill_block(title_draw_x, title_draw_y, COLOR_TITLEBAR, title_draw_w, title_draw_h);
    }

    // 3. Rysowanie tekstu tytułu (tylko gdy pasek jest widoczny na ekranie)
    if (window->pos_x + 10 < (int)fb->width && window->pos_y + (title_height - 8) / 2 < (int)fb->height) {
        print_at8(window->name, window->pos_x + 10, window->pos_y + (title_height - 8) / 2, COLOR_WHITE);
    }

    // 4. Rysowanie przycisków kontrolnych
    int button_x = window->pos_x + window->width - 55;
    int button_y = window->pos_y + (title_height - 8) / 2;

    if (button_x < (int)fb->width && button_y < (int)fb->height) {
        print_at8("-", button_x, button_y, COLOR_WHITE);
        print_at8("[]", button_x + 16, button_y, COLOR_WHITE);
        print_at8("X", button_x + 42, button_y, COLOR_WHITE);
    }
}

bool is_mouse_over_window(window_struct* window, int mouse_x, int mouse_y) {
    if (!window || !window->visible)
        return false;

    return (mouse_x >= window->pos_x &&
            mouse_x < window->pos_x + window->width &&
            mouse_y >= window->pos_y &&
            mouse_y < window->pos_y + window->height);
}

bool is_mouse_over_window_title(window_struct* window, int mouse_x, int mouse_y) {
    if (!window || !window->visible)
        return false;

    int title_height = window->height / 10;
    if (title_height < 18) title_height = 18;

    return (mouse_x >= window->pos_x &&
            mouse_x < window->pos_x + window->width &&
            mouse_y >= window->pos_y &&
            mouse_y < window->pos_y + title_height);
}

window_button get_window_button(window_struct* window, int mouse_x, int mouse_y) {
    if (!window || !window->visible)
        return BUTTON_NONE;

    int title_height = window->height / 10;
    if (title_height < 18) title_height = 18;

    // Zwiększamy margines kliknięcia (hitbox) w pionie do pełnej wysokości paska tytułowego
    if (mouse_y >= window->pos_y && mouse_y < window->pos_y + title_height) {
        int minimize_start = window->pos_x + window->width - 58; // Lekki margines w lewo
        
        // Obliczamy pozycję X względem początku strefy przycisków dla łatwiejszego dopasowania
        int rel_x = mouse_x - minimize_start;

        // Przycisk Minimalizuj (-) : szerokość ok. 14 pikseli w strefie
        if (rel_x >= 0 && rel_x < 14) {
            return BUTTON_MINIMIZE;
        }
        // Przycisk Maksymalizuj ([]) : szerokość ok. 22 pikseli w strefie
        if (rel_x >= 14 && rel_x < 36) {
            return BUTTON_MAXIMIZE;
        }
        // Przycisk Zamknij (X) : szerokość ok. 20 pikseli w strefie
        if (rel_x >= 36 && rel_x < 56) {
            return BUTTON_CLOSE;
        }
    }

    return BUTTON_NONE;
}
