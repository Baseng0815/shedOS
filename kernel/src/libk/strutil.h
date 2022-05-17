#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define NPOS SIZE_MAX

size_t strlen(const char *str);

/* unsigned integer to string conversion
 * the value part of the result is guaranteed to be at least min_len long
 */
size_t utos(uint64_t i, uint64_t base, char *buf, size_t min_len);

/* string to unsigned integer conversion */
uint64_t stou(const char *str, uint64_t base);

char *strtok(char *s, char delim);

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

size_t strfind(const char *s, char c);

size_t *strcpy(char *dst, const char *src);

char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);

size_t trim_and_terminate(char *src, size_t max_n);

bool isdigit(char c);

#endif
