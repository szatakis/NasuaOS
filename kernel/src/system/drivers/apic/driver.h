#pragma once

#include <stdint.h>


void apic_disable();
uint64_t apic_read_base();
bool apic_enabled();