#pragma once

#include <stdint.h>

#define PAGE_PRESENT 0x001
#define PAGE_WRITE   0x002
#define PAGE_USER    0x004
#define PAGE_NX      (1ULL << 63)

void vmm_init();
bool vmm_map_page(uint64_t virt, uint64_t phys, uint64_t flags);
bool vmm_unmap_page(uint64_t virt);
uint64_t vmm_translate(uint64_t virt);