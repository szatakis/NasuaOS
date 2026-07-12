#pragma once

#include <stdint.h>
#include <cstdint>
#include <cstddef>

// Deklaracje funkcji mapujących kody skanów na ASCII
char scancode_to_ascii_normal(uint8_t scancode);
char scancode_to_ascii_shift(uint8_t scancode);
void print_sc(uint8_t scancode);

// Główna funkcja do obsługi wejścia z klawiatury.
// Wywołuj ją w głównej pętli systemu.
void handle_keyboard();

static bool shift_pressed = false;
static bool extended_scancode = false;

extern char command_buffer[64];
extern size_t cmd_idx;