#include "../driver.h"
#include <stdint.h>

static inline void cpuid(uint32_t leaf, uint32_t subleaf, uint32_t& eax, uint32_t& ebx, uint32_t& ecx, uint32_t& edx)
{
    asm volatile(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(leaf), "c"(subleaf)
    );
}

void cpu_get_brand(char* brand)
{
    uint32_t eax, ebx, ecx, edx;

    char* ptr = brand;

    for (uint32_t leaf = 0x80000002; leaf <= 0x80000004; leaf++)
    {
        cpuid(leaf, 0, eax, ebx, ecx, edx);

        *(uint32_t*)(ptr + 0)  = eax;
        *(uint32_t*)(ptr + 4)  = ebx;
        *(uint32_t*)(ptr + 8)  = ecx;
        *(uint32_t*)(ptr + 12) = edx;

        ptr += 16;
    }

    brand[48] = '\0';
}