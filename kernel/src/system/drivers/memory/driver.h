#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <cstdint>
#include <cstddef>

#include "functions/heap.h"
#include "functions/paging.h"
#include "functions/pmm.h"
#include "functions/vmm.h"

void memory_init();
uint64_t memory_used();
const char* memory_total();
uint64_t memory_total_bytes();

extern "C" {
    void *memcpy(void *__restrict dest, const void *__restrict src, std::size_t n);
    void *memset(void *s, int c, std::size_t n);
    void *memmove(void *dest, const void *src, std::size_t n);
    int memcmp(const void *s1, const void *s2, std::size_t n);
}

#endif