#include <stdint.h>
#include <cstddef>

void strcpy(char* dest, const char* src)
{
    while (*src)
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* itoa(int64_t value, char* str)
{
    char* p = str;
    bool negative = false;

    if (value == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    // Obsługa liczb ujemnych na typie 64-bitowym
    if (value < 0)
    {
        negative = true;
        // Używamy uint64_t do bezpiecznej konwersji wartości bezwzględnej.
        // Zapobiega to błędowi przepełnienia dla najmniejszej możliwej liczby.
        uint64_t abs_value = -static_cast<uint64_t>(value);
        
        while (abs_value)
        {
            *p++ = '0' + (abs_value % 10);
            abs_value /= 10;
        }
    }
    else
    {
        uint64_t abs_value = value;
        while (abs_value)
        {
            *p++ = '0' + (abs_value % 10);
            abs_value /= 10;
        }
    }

    if (negative)
    {
        *p++ = '-';
    }

    *p = '\0';

    // Bezpieczne odwrócenie napisu (algorytm z u64toa)
    char* start = str;
    char* end = p - 1;

    while (start < end)
    {
        char t = *start;
        *start = *end;
        *end = t;
        start++;
        end--;
    }

    return str;
}


// Uniwersalna funkcja parsująca określoną liczbę cyfr (od 1 do np. 10 cyfr dla uint32_t)
uint32_t parse_digits(const char* str, uint8_t count) {
    uint32_t result = 0;

    for (uint8_t i = 0; i < count; i++) {
        // Zabezpieczenie: jeśli tekst skończy się za wcześnie lub trafimy na znak niebędący cyfrą
        if (str[i] < '0' || str[i] > '9') {
            return result; // Przerywamy i zwracamy to, co udało się ustalić
        }
        
        result = result * 10 + (str[i] - '0');
    }

    return result;
}

size_t shell_strlen(const char *str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

bool shell_strncmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return false;
        if (s1[i] == '\0') return true;
    }
    return true;
}

const char* shell_strchr(const char *str, char c) {
    while (*str) {
        if (*str == c) return str;
        str++;
    }
    return nullptr;
}

// Szuka podciągu (np. "--color") wewnątrz całych argumentów (odpowiednik strstr)
const char* shell_strstr(const char *haystack, const char *needle) {
    if (!*needle) return haystack;
    while (*haystack) {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && *h == *n) {
            h++;
            n++;
        }
        if (!*n) return haystack;
        haystack++;
    }
    return nullptr;
}

// DODANE: Konwertuje pojedynczy znak HEX ('0'-'9', 'A'-'F', 'a'-'f') na wartość 0-15
uint8_t hex_char_to_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

// DODANE: Konwertuje string "0xRRGGBB" lub "RRGGBB" na liczbę uint32_t
uint32_t parse_hex_color(const char *str) {
    // Jeśli tekst zaczyna się od "0x" lub "0X", pomiń te 2 znaki
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }
    
    // Jeśli tekst zaczyna się od cudzysłowu '"', również go pomiń
    if (*str == '"') {
        str++;
    }

    uint32_t result = 0;
    for (int i = 0; i < 8; i++) { // Maksymalnie 8 znaków dla formatu RRGGBB lub ARGB
        char c = str[i];
        if (c == '\0' || c == '"' || c == ' ') break;
        
        result <<= 4;
        result |= hex_char_to_val(c);
    }
    return result;
}

int parse_number(const char* str) {
    int num = 0;

    while (*str >= '0' && *str <= '9') {
        num = num * 10 + (*str - '0');
        str++;
    }

    return num;
}

void to_hex8(uint8_t value, char* out) {
    static const char hex[] = "0123456789ABCDEF";

    out[0] = '0';
    out[1] = 'x';
    out[2] = hex[(value >> 4) & 0xF];
    out[3] = hex[value & 0xF];
    out[4] = '\0';
}

size_t strlen(const char* str)
{
    size_t length = 0;

    while (str[length] != '\0')
    {
        length++;
    }

    return length;
}

char* strcat(char* destination, const char* source)
{
    char* ptr = destination;

    // znajdź koniec destination
    while (*ptr != '\0')
    {
        ptr++;
    }

    // skopiuj source na koniec destination
    while (*source != '\0')
    {
        *ptr = *source;
        ptr++;
        source++;
    }

    // dodaj koniec stringa
    *ptr = '\0';

    return destination;
}

int atoi(const char* str) {
    int res = 0;
    int sign = 1;
    int i = 0;

    // 1. Pomiń białe znaki na początku (jeśli istnieją)
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || 
           str[i] == '\r' || str[i] == '\v' || str[i] == '\f') {
        i++;
    }

    // 2. Obsługa znaku plus / minus
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    // 3. Konwersja kolejnych cyfr na liczbę (poprawiony warunek na '9')
    while (str[i] >= '0' && str[i] <= '9') {
        res = res * 10 + (str[i] - '0');
        i++;
    }

    return sign * res;
}