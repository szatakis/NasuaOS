#pragma once
#include <stdint.h>
#include <stddef.h>

// Deklaracje funkcji do odczytu i zapisu sektorów.
// Parametr 'lba' (Logical Block Addressing) to numer sektora na dysku (od 0).
// Parametr 'buffer' to wskaźnik na tablicę 512 bajtów.

void disk_read_sector(uint32_t lba, uint8_t* buffer);
void disk_write_sector(uint32_t lba, uint8_t* buffer);

// Inicjalizuje podsystem pomiaru pojemności dysku
void storage_init();

// Zwraca całkowitą pojemność dysku w bajtach
uint64_t storage_total();

// Zwraca zużytą przestrzeń na dysku w bajtach (na podstawie systemu plików)
uint64_t storage_used();