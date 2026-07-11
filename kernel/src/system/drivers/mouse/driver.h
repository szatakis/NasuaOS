#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

#define CURSOR_W 12
#define CURSOR_H 19

// Deklaracja - mówisz kompilatorowi: "zmienna varA istnieje i znajduje się w innym pliku"
extern int32_t mouse_x;
extern int32_t mouse_y;

// Definicja wyglądu systemowej strzałki
extern const char arrow_cursor[CURSOR_H][CURSOR_W];
extern uint32_t mouse_backdrop[CURSOR_W * CURSOR_H];

#endif