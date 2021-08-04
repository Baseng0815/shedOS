#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

size_t strlen(const char *str);

/* unsigned integer to string conversion
 * the value part of the result is guaranteed to be at least min_len long
 */
size_t utos(uint64_t i, uint64_t base, char *buf, size_t min_len);

/* string to unsigned integer conversion */
uint64_t stou(const char *str, uint64_t base);

size_t strcmp(const char *s1, const char *s2);
size_t strncmp(const char *s1, const char *s2, size_t n);

size_t strcpy(const char *src, char *dst);
size_t strncpy(const char *src, char *dst, size_t n);

size_t trim_and_terminate(char *src, size_t max_n);

bool isdigit(char c);

#endif
