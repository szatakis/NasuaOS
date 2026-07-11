#include "../driver.h"

#include "system/drivers/uart/driver.h"
#include "kernel/include/logger/logger.hpp"


#define IA32_APIC_BASE_MSR 0x1B



uint64_t apic_read_base()
{
    uint32_t eax;
    uint32_t edx;

    asm volatile(
        "mov %2, %%ecx\n"
        "rdmsr"
        : "=a"(eax), "=d"(edx)
        : "r"(IA32_APIC_BASE_MSR)
        : "ecx"
    );

    return ((uint64_t)edx << 32) | eax;
}

bool apic_enabled()
{
    Uart::puts("[APIC] Enabling...\n");
    log(INFO,"APIC","Enabling...");
    uint64_t base = apic_read_base();

    return base & (1ULL << 11);
    Uart::puts("[APIC] Enabled\n");
    log(INFO,"APIC","Enabled");
}

void apic_disable()
{
    Uart::puts("[APIC] Disabling...\n");
    log(INFO,"APIC","Disabling...");

    uint64_t base = apic_read_base();

    if(!(base & (1ULL << 11)))
    {
        Uart::puts("[APIC] Already disabled\n");
        log(INFO,"APIC","Already disabled");
        return;
    }

    base &= ~(1ULL << 11);

    uint32_t eax = base & 0xFFFFFFFF;
    uint32_t edx = base >> 32;

    asm volatile(
        "mov %0, %%eax\n"
        "mov %1, %%edx\n"
        "mov $0x1B, %%ecx\n"
        "wrmsr"
        :
        : "r"(eax),
          "r"(edx)
        : "eax",
          "edx",
          "ecx"
    );

    Uart::puts("[APIC] Disabled\n");
    log(INFO,"APIC","Disabled");
}