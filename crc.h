#ifndef CRC_H
#define CRC_H

#include <stdint.h>

#define M(X) (0x00000001 << (X))

uint32_t reverse(uint32_t);
uint8_t reverse8(uint8_t);

void gen_table(uint32_t *, uint32_t, int);

#endif
