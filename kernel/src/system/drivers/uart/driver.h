#pragma once
#include <cstdint>

class Uart {
private:
    static constexpr uint16_t COM1_PORT = 0x3F8;

    // Prywatne metody niskopoziomowe do obsługi portów I/O
    static inline void outb(uint16_t port, uint8_t val) 
    {
        asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
    }

    static inline uint8_t inb(uint16_t port) 
    {
        uint8_t ret;
        asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }

    static inline bool is_transmit_empty() 
    {
        return (inb(COM1_PORT + 5) & 0x20) != 0;
    }

public:
    // Blokujemy tworzenie instancji - to jest czysta klasa statyczna
    Uart() = delete;

    static void init();
    static void putc(char c);
    static void puts(const char* str);
    
    // Dodatkowa metoda w C++ ułatwiająca wypisywanie liczb szesnastkowych (przydatne do debugowania adresów)
    static void puthex(uint64_t val);
    static void putdec(uint64_t val);
};
