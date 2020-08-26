#include "crc.h"

#include <stdio.h>

uint32_t reverse(uint32_t n) {
  uint32_t result, i;
  for (i = 0, result = 0; i < 32; i++) {
    if (n & M(i)) {
      result |= M(31 - i);
    }
  }
  return result;
}

uint8_t reverse8(uint8_t n) {
  uint8_t result, i;
  for (i = 0, result = 0; i < 8; i++) {
    if (n & M(i)) {
      result |= M(7 - i);
    }
  }
  return result;
}

void gen_table(uint32_t *t, uint32_t polynomial, int reversed) {
  uint16_t i;
  for (i = 0; i < 256; i++) {
    uint32_t b;
    uint16_t i_r = reverse8((uint8_t)i);
    if (reversed) {
      b = (i_r << 24) & 0xFFFFFFFF;
    } else {
      b = (i << 24) & 0xFFFFFFFF;
    }
    uint8_t bit;
    for (bit = 0; bit < 8; bit++) {
      if ((b & M(31)) != 0) {
	b <<= 1;
	b ^= polynomial;
      } else {
	b <<= 1;
      }
    }
    t[i] = reversed ? reverse(b) : b;
  }
}

uint32_t calc_crc(uint32_t *table, uint32_t polynomial, uint8_t *data, size_t size, crc_model * m) {
  uint32_t crc = m -> init;
  size_t i;
  for (i = 0; i < size; i++) {
    uint8_t b = data[i];
    if (m -> refl_in) {
      b = reverse8(b);
    }
    crc = (crc ^ (b << (m -> width - 8)));
    uint8_t ind = (crc >> (m -> width - 8));
    crc = crc << 8;
    crc = (crc ^ table[ind]);
  }
  if (m -> refl_out) {
    crc = reverse(crc);
  }
  return crc ^ m -> xor_out;
}
