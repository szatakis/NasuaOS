#pragma once

#include <stdint.h>

void pic_remap();
void pic_disable();
void pic_send_eoi(uint8_t irq);

// Wysyla EOI do wlasciwego kontrolera przerwan (LAPIC jesli aktywny,
// w przeciwnym razie 8259 PIC). Uzywaj tego zamiast pic_send_eoi()
// w handlerach IRQ, zeby dzialaly niezaleznie od trybu.
void irq_send_eoi(uint8_t irq);
