#pragma once
#include <stdint.h>

struct MadtInfo
{
    bool valid;                // MADT zostal znaleziony i wyglada poprawnie
    bool has_ioapic;           // MADT zawiera wpis I/O APIC (typ 1)
    uint64_t ioapic_base;      // fizyczny adres I/O APIC
    uint32_t ioapic_gsi_base;  // GSI base tego I/O APIC
    uint32_t irq0_gsi;         // GSI odpowiadajacy legacy ISA IRQ0
                                // (z uwzglednieniem Interrupt Source Override,
                                // domyslnie = 0 jesli override nie wystepuje)
};

// Parsuje ACPI RSDP -> RSDT/XSDT -> MADT. Zwraca valid=false jesli
// RSDP/MADT nie zostal znaleziony (wtedy nalezy bezpiecznie spasc
// na legacy 8259 PIC zamiast zgadywac adresy APIC).
MadtInfo madt_parse();
