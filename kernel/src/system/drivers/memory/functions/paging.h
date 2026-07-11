#pragma once

#include <stdint.h>


void paging_init();
uint64_t get_cr3();
uint64_t get_hhdm_offset();