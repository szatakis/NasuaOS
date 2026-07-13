#include "../driver.h"
#include "system/fonts/font8x8.h"
#include "system/fonts/font10x10.h"
#include "system/fonts/font12x12.h"
#include "system/fonts/font16x16.h"
#include "system/gui/vars/colors.h"
#include "applications/shell/commands.h"
#include "libs/libc/libc.h"
#include "system/drivers/keyboard/driver.h"

// Wymiary bufora terminala na podstawie standardowej czcionki 8x8
#define TERM_COLS 160  // Maksymalna liczba znaków w linii
#define TERM_ROWS 80   // Maksymalna liczba linii tekstu w historii

// Struktura reprezentująca jeden znak na ekranie wraz z jego kolorem
typedef struct {
    char ch;
    uint32_t color;
} TermChar;

// Globalny bufor terminala, który przechowuje aktualną historię tekstu
static TermChar term_buffer[TERM_ROWS][TERM_COLS];

// Wirtualna pozycja kursora w buforze tekstowym
static size_t term_col = 0;
static size_t term_row = 0;

uint32_t current_text_color = COLOR_WHITE;

// Zachowujemy te zmienne dla kompatybilności z Twoim OS
size_t cursor_x = 0;
size_t cursor_y = 0;

const uint32_t FONT_SPACING_W = 1;
const uint32_t FONT_SPACING_H = 4;

extern char current_path[256];
extern char user_name[16];
extern char pc_name[16];

// Funkcja czyszcząca pamięć bufora tekstowego (wywołaj raz przy starcie systemu oraz przy clear_screen)
void init_terminal_buffer() {
    // Resetowanie wirtualnej pozycji kursora na samą górę ekranu
    term_col = 0;
    term_row = 0;

    // Czyszczenie całej macierzy znaków
    for (size_t r = 0; r < TERM_ROWS; r++) {
        for (size_t c = 0; c < TERM_COLS; c++) {
            term_buffer[r][c].ch = ' ';
            term_buffer[r][c].color = COLOR_WHITE;
        }
    }
}

// Funkcja przewijająca tekst w górę, gdy zabraknie miejsca na dole
static void scroll_terminal() {
    for (size_t r = 1; r < TERM_ROWS; r++) {
        for (size_t c = 0; c < TERM_COLS; c++) {
            term_buffer[r - 1][c] = term_buffer[r][c];
        }
    }
    // Wyczyszczenie ostatniej linii
    for (size_t c = 0; c < TERM_COLS; c++) {
        term_buffer[TERM_ROWS - 1][c].ch = ' ';
        term_buffer[TERM_ROWS - 1][c].color = COLOR_WHITE;
    }
    if (term_row > 0) {
        term_row--;
    }
}

// Rysuje cały zapisany tekst z bufora tekstowego bezpośrednio do backbuffera
// WYWOŁAJ TO W KAŻDEJ KLATCE!
void draw_terminal_buffer() {
    size_t start_y = 0;
    
    for (size_t r = 0; r < TERM_ROWS; r++) {
        size_t start_x = 0;
        
        // Sprawdzenie, czy nie wchodzimy na dolny pasek zadań
        if (start_y + 8 >= (fb->height - 45)) break;

        for (size_t c = 0; c < TERM_COLS; c++) {
            if (start_x + 8 >= fb->width) break;

            char ch = term_buffer[r][c].ch;
            // Rysujemy tylko rzeczywiste znaki (spacje ignorujemy lub nadpisujemy tłem)
            if (ch != ' ' && ch != '\0') {
                draw_char8(ch, start_x, start_y, term_buffer[r][c].color);
            }
            
            start_x += 8 + FONT_SPACING_W;
        }
        start_y += 8 + FONT_SPACING_H;
    }
    
    // Aktualizacja fizycznych pozycji kursora na potrzeby renderowania promptu i pisania
    cursor_x = term_col * (8 + FONT_SPACING_W);
    cursor_y = term_row * (8 + FONT_SPACING_H);
}

void print_char8(char c) {
    if (c == '\n') {
        term_col = 0;
        term_row++;
        
        // Obliczamy limit linii na podstawie wysokości ekranu i paska dolnego
        size_t max_rows = (fb->height - 45) / (8 + FONT_SPACING_H);
        if (term_row >= max_rows || term_row >= TERM_ROWS) {
            scroll_terminal();
        }
        return;
    }

    // Maksymalna szerokość w znakach
    size_t max_cols = fb->width / (8 + FONT_SPACING_W);
    if (term_col >= max_cols || term_col >= TERM_COLS) {
        print_char8('\n');
    }

    // Zapis znaku i koloru do pamięci bufora tekstowego
    term_buffer[term_row][term_col].ch = c;
    term_buffer[term_row][term_col].color = current_text_color;
    
    term_col++;
}

void print_num8(uint32_t num) {
    char buffer[12];
    int i = 0;

    if (num == 0) {
        print("0");
        return;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0) {
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

void print_int(int value) {
    char buffer[32];
    int i = 0;

    if (value == 0) {
        print("0");
        return;
    }

    if (value < 0) {
        print("-");
        value = -value;
    }

    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        char c[2];
        c[0] = buffer[j];
        c[1] = '\0';
        print(c);
    }
}

// Funkcje print_at ignorują bufor terminala i rysują "sztywny" tekst na ekranie co klatkę
void print_at8(const char* str, size_t x, size_t y, uint32_t color) {
    size_t start_x = x;
    while (*str) {
        if (*str == '\n') {
            x = start_x;
            y += 8 + FONT_SPACING_H;
            str++;
            continue;
        }
        draw_char8(*str, x, y, color);
        x += 8 + FONT_SPACING_W;
        str++;
    }
}

void print_at10(const char* str, size_t x, size_t y, uint32_t color) {
    size_t start_x = x;
    while (*str) {
        if (*str == '\n') {
            x = start_x;
            y += 10 + FONT_SPACING_H;
            str++;
            continue;
        }
        draw_char10(*str, x, y, color);
        x += 10 + FONT_SPACING_W;
        str++;
    }
}

void print_at12(const char* str, size_t x, size_t y, uint32_t color) {
    size_t start_x = x;
    while (*str) {
        if (*str == '\n') {
            x = start_x;
            y += 12 + FONT_SPACING_H;
            str++;
            continue;
        }
        draw_char12(*str, x, y, color);
        x += 12 + FONT_SPACING_W;
        str++;
    }
}

void print_at16(const char* str, size_t x, size_t y, uint32_t color) {
    size_t start_x = x;
    while (*str) {
        if (*str == '\n') {
            x = start_x;
            y += 16 + FONT_SPACING_H;
            str++;
            continue;
        }
        draw_char16(*str, x, y, color);
        x += 16 + FONT_SPACING_W;
        str++;
    }
}

// Backspace usuwa teraz znak bezpośrednio z bufora tekstowego w pamięci
void delete_last_char() {
    if (term_col > 0) {
        term_col--;
        term_buffer[term_row][term_col].ch = ' ';
    } else if (term_row > 0) {
        term_row--;
        // Przeskakujemy na koniec poprzedniej linii
        size_t max_cols = fb->width / (8 + FONT_SPACING_W);
        term_col = (max_cols < TERM_COLS) ? max_cols - 1 : TERM_COLS - 1;
        term_buffer[term_row][term_col].ch = ' ';
    }
}

// Czyszczenie aktualnie wpisywanej linii z bufora tekstowego
void clear_line() {
    size_t start_col = term_col - cmd_idx;
    
    for (size_t c = start_col; c < term_col; c++) {
        term_buffer[term_row][c].ch = ' ';
    }
    
    term_col = start_col;

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
    print("\n $$\\   $$\\                                          $$$$$$\\   $$$$$$\\  \n");
    print(" $$$\\  $$ |                                        $$  __$$\\ $$  __$$\\ \n");
    print(" $$$$\\ $$ | $$$$$$\\   $$$$$$$\\ $$\\   $$\\  $$$$$$\\  $$ /  $$ |$$ /  \\__|\n");
    print(" $$$$\\ $$ | $$$$$$\\   $$$$$$$\\ $$\\   $$\\  $$$$$$\\  $$ /  $$ |$$ /  \\__|\n");
    print(" $$ $$\\$$ | \\____$$\\ $$  _____|$$ |  $$ | \\____$$\\ $$ |  $$ |\\$$$$$$\\  \n");
    print(" $$ \\$$$$ | $$$$$$$ |\\$$$$$$\\  $$ |  $$ | $$$$$$$ |$$ |  $$ | \\____$$\\ \n");
    print(" $$ |\\$$$ |$$  __$$ | \\____$$\\ $$ |  $$ |$$  __$$ |$$ |  $$ |$$\\   $$ |\n");
    print(" $$ | \\$$ |\\$$$$$$$ |$$$$$$$  |\\$$$$$$  |\\$$$$$$$ | $$$$$$  |\\$$$$$$  |\n");
    print(" \\__|  \\__| \\_______|\\_______/  \\______/  \\_______| \\______/  \\______/ \n\n");
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

void print_num_padded(uint32_t num) {
    char buf[3];
    buf[0] = '0' + (num / 10);
    buf[1] = '0' + (num % 10);
    buf[2] = '\0';
    print(buf);
}

void print_year(uint32_t year) {
    char buf[5];
    buf[0] = '0' + (year / 1000);
    buf[1] = '0' + ((year % 1000) / 100);
    buf[2] = '0' + ((year % 100) / 10);
    buf[3] = '0' + (year % 10);
    buf[4] = '\0';
    print(buf);
}

void print_resolution() {
    if (!fb) {
        print("Framebuffer not available\n");
        return;
    }
    print_info("Screen resolution: ");
    print_int(fb->width);
    print("x");
    print_int(fb->height);
    print("\n");
}
