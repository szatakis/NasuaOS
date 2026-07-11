#pragma once
#include <stddef.h>
#include <stdint.h>
#include <cstddef>

void strcpy(char* dest, const char* src);
int strcmp(const char* s1, const char* s2);
size_t strlen(const char* str);
char* strcat(char* destination, const char* source);
char* itoa(int64_t value, char* str);
uint32_t parse_digits(const char* str, uint8_t count);

size_t shell_strlen(const char *str);
bool shell_strncmp(const char *s1, const char *s2, size_t n);
const char* shell_strchr(const char *str, char c);
const char* shell_strstr(const char *haystack, const char *needle);

uint8_t hex_char_to_val(char c);
uint32_t parse_hex_color(const char *str);
int parse_number(const char* str);

void to_hex8(uint8_t value, char* out);