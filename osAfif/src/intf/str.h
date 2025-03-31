#pragma once

#include <stdint.h>
#include <stddef.h>

// String.h file, renamed to str.h
#define LONG_MASK (sizeof(unsigned long) - 1)

size_t strlen(const char* str);
void* memset(void* bufptr, int value, size_t size);
int memcmp(const void* aptr, const void* bptr, size_t n);
void* memmove(void* dest, const void* src, size_t n);
void *memcpy(void* restrict dest, const void* restrict src, size_t n);
int strcmp(const char* s1, const char* s2);
char* strcpy(char* dest, const char* src);
char* strcat(char* dest, const char* src);
char* strrev(char* str);
char* itoa(int value, char* str, int base);
char* utoa(unsigned int value, char* str, int base);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);

//Mathematic functions
int abs(int n);
int pow(int base, int exp);
int max(int a, int b);
int min(int a, int b);
int isdigit(int c);
int isalpha(int c);
int isalnum(int c);
int islower(int c);
int isupper(int c);
int tolower(int c);
int toupper(int c);
int sin(int x);
int cos(int x);
int tan(int x);
int asin(int x);
int acos(int x);
int atan(int x);
int square_root(int x);
int log(int x);
int quadratic(int a, int b, int c);
int factorial(int n);
int gcd(int a, int b);
int lcm(int a, int b);
int isprime(int n);
int powf(int base, int exp);



