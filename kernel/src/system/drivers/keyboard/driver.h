#pragma once

#include <stdint.h>

// Deklaracje funkcji mapujących kody skanów na ASCII
char scancode_to_ascii_normal(uint8_t scancode);
char scancode_to_ascii_shift(uint8_t scancode);
void print_sc(uint8_t scancode);