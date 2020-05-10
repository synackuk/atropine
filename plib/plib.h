#ifndef PLIB_H
#define PLIB_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <printf.h>

typedef void(*putc_t)(char c);

char* strcat(char * __restrict s, const char * __restrict append);
char* strncat(char *dst, const char *src, size_t n);
char* strcpy(char * __restrict to, const char * __restrict from);
size_t strlen(const char *str);
int atoi(const char * s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
int memcmp(const void *b1, const void *b2, size_t len);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmem(const void* haystack, size_t hlen, const void* needle, size_t nlen);
char* ultoa(unsigned long val, char* s, int radix);
void panic(char* err, ...);
void clear_icache();
void enable_unaligned_access();
void disable_unaligned_access();
void disable_irq();

void add_putc(putc_t handler);
void _putchar(char c);
#define putchar _putchar

#endif