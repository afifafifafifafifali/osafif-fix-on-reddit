#include "str.h"
#include <stdint.h>
#include <stddef.h>

// String functions

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void* memset(void* bufptr, int value, size_t size) {
    unsigned char* p = bufptr;
    while (size--) {
        *p++ = (unsigned char)value;
    }
    return bufptr;
}

int memcmp(const void* aptr, const void* bptr, size_t n) {
    const unsigned char* a = aptr;
    const unsigned char* b = bptr;
    while (n--) {
        if (*a != *b) {
            return (*a - *b);
        }
        a++;
        b++;
    }
    return 0;
}

void* memmove(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    if (s < d && d < s + n) {
        s += n;
        d += n;
        while (n--) {
            *(--d) = *(--s);
        }
    } else {
        while (n--) {
            *d++ = *s++;
        }
    }
    return dest;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) {
        d++;
    }
    while ((*d++ = *src++) != '\0');
    return dest;
}

char* strrev(char* str) {
    char* start = str;
    char* end = str + strlen(str) - 1;
    char temp;
    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
    return str;
}

char* itoa(int value, char* str, int base) {
    int i = 0;
    int isNegative = (value < 0 && base == 10);  // Only negative for base 10

    if (isNegative) {
        value = -value;
    }

    do {
        str[i++] = "0123456789ABCDEF"[value % base];
        value /= base;
    } while (value > 0);

    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Reverse string
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }

    return str;
}


char* utoa(unsigned int value, char* str, int base) {
    int i = 0;
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    while (value != 0) {
        int rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value = value / base;
    }
    str[i] = '\0';
    strrev(str);
    return str;
}

char* strchr(const char* s, int c) {
    while (*s != '\0') {
        if (*s == c) {
            return (char*)s;
        }
        s++;
    }
    return NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s != '\0') {
        if (*s == c) {
            last = s;
        }
        s++;
    }
    return (char*)last;
}

// Mathematical functions

int abs(int n) {
    return (n < 0) ? -n : n;
}

int pow(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

int isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int isalpha(int c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

int isalnum(int c) {
    return (isalpha(c) || isdigit(c));
}

int islower(int c) {
    return (c >= 'a' && c <= 'z');
}

int isupper(int c) {
    return (c >= 'A' && c <= 'Z');
}

int tolower(int c) {
    return (isupper(c)) ? c + ('a' - 'A') : c;
}

int toupper(int c) {
    return (islower(c)) ? c - ('a' - 'A') : c;
}

int sin(int x) {
    // Simplified sine function approximation (for small angles)
    return x - (x * x * x) / 6;
}

int cos(int x) {
    // Simplified cosine function approximation (for small angles)
    return 1 - (x * x) / 2;
}

int tan(int x) {
    return sin(x) / cos(x);
}

int asin(int x) {
    return x;  // Inverse sine approximation (not accurate)
}

int acos(int x) {
    return x;  // Inverse cosine approximation (not accurate)
}

int atan(int x) {
    return x;  // Inverse tangent approximation (not accurate)
}

int square_root(int x) {
    int result = 0;
    while (result * result <= x) {
        result++;
    }
    return result - 1;
}

int log(int x) {
    int result = 0;
    while (x >= 2) {
        x /= 2;
        result++;
    }
    return result;
}

int quadratic(int a, int b, int c) {
    int discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1; // No real roots
    }
    int root1 = (-b + square_root(discriminant)) / (2 * a);
    int root2 = (-b - square_root(discriminant)) / (2 * a);
    return root1 > root2 ? root1 : root2; // Return larger root
}

int factorial(int n) {
    if (n == 0 || n == 1) {
        return 1;
    }
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int lcm(int a, int b) {
    return (a * b) / gcd(a, b);
}

int isprime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int powf(int base, int exp) {
    return pow(base, exp); // For simplicity, integer exponentiation only
}
