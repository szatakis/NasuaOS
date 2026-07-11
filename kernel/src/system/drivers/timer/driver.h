#pragma once

#include <stdint.h>


void pit_init();
void pit_handler();
uint64_t pit_get_ticks();
void sleep(uint64_t ms);