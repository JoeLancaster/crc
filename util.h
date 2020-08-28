#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>

size_t dump_file (char *, uint8_t **, size_t);
void print_hex (int, uint64_t);
void strupper (char *);

#endif
