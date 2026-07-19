#pragma once
#include <stdint.h>

// ---------------- CPUID / dostępność ----------------

// true jesli CPU zglasza wsparcie dla Local APIC (CPUID.1:EDX.APIC[9])
bool apic_available();

// ---------------- Local APIC (istniejace + nowe) ----------------

uint64_t apic_read_base();
bool apic_enabled();

void apic_enable();   // ustawia bit Global Enable w IA32_APIC_BASE
void apic_disable();  // czysci bit Global Enable w IA32_APIC_BASE

void lapic_init();       // wlacza LAPIC (Spurious Vector Register) i zeruje TPR
void lapic_send_eoi();   // wysyla EOI do Local APIC

// ---------------- I/O APIC ----------------

void ioapic_init();
void ioapic_mask_irq(uint8_t irq);
void ioapic_set_irq(uint8_t irq, uint8_t vector, uint32_t dest_apic_id);

// ---------------- Wybor kontrolera przerwan ----------------

// true jesli kernel biezaco korzysta z APIC (LAPIC+IOAPIC) zamiast 8259 PIC
bool apic_is_active();

// Wykrywa dostepnosc APIC. Jesli jest dostepny - uzywa LAPIC+IOAPIC (nowsze PC).
// Jesli nie - wraca do 8259 PIC (starsze PC / QEMU bez APIC).
void interrupts_controller_init();
