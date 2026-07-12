// Zmienna przechowująca stan generatora (ziarno)
static unsigned long next_random = 1;

// Funkcja odczytująca licznik cykli procesora (Timestamp Counter)
// Działa na każdym procesorze x86 od czasów Pentium
static inline unsigned long long read_tsc(void) {
    unsigned int lo, hi;
    // Wywołanie instrukcji assemblera rdtsc
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)hi << 32) | lo;
}

/**
 * Inicjalizuje generator liczb losowych dynamicznym ziarnem.
 * Najlepiej wywołać ją raz podczas startu powłoki (shella) lub jądra.
 */
void system_srand(void) {
    // Jako ziarno wykorzystujemy aktualną liczbę cykli procesora od startu komputera
    next_random = (unsigned long)read_tsc();
}

/**
 * Zwraca liczbę pseudolosową z zakresu od 0 do 32767.
 * Wykorzystuje sprawdzony algorytm LCG (używany m.in. w bibliotece glibc).
 */
int system_rand(void) {
    // Jeśli użytkownik zapomniał wywołać system_srand(), inicjalizujemy automatycznie
    if (next_random == 1) {
        next_random = (unsigned long)read_tsc();
    }

    // Klasyczny wzór LCG: X_{n+1} = (A * X_n + C) % M
    next_random = next_random * 1103515245 + 12345;
    
    // Zwracamy wynik skrócony do wartości dodatniej (standardowy zakres POSIX)
    return (int)(next_random / 65536) % 32768;
}
