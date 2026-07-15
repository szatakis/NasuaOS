#pragma once

#include <stdint.h>

#define PAGE_SIZE 4096

void pmm_init();
uint64_t pmm_alloc_page();
void pmm_free_page(uint64_t addr);
uint64_t pmm_get_total_memory();