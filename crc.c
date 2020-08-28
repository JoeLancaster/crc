#include "crc.h"
#include <stdio.h>
#include <stdlib.h>

#define M(X) (0x00000001 << (X))
#define WM(X) ( (1 << (X)) - 1) 

uint64_t make_mask (int width) {
  uint64_t mask = 1;
  mask = width == 64 ? (uint64_t)0xFFFFFFFFFFFFFFFF : ((mask << width) - 1);
  return mask;
}

uint64_t reverse (uint64_t n, int width) {
  uint64_t result;
  int i;
  for (i = 0, result = 0; i < width; i++) {
    if (n & M(i)) {
      result |= M(width - i - 1);
    }
  }
  return result;
}

void gen_table (uint64_t *t, int reversed, crc_model *m) {
  int width = m -> width;
  uint64_t polynomial = m -> poly;
  const uint64_t wmb = width - 8;
  uint64_t mask = make_mask(width);
  
  uint16_t i;
  uint8_t bit;
  for (i = 0; i < 256; i++) {
    
    uint64_t b = ((reversed ? reverse(i, 8) : i) << wmb ) & mask;
    
    for (bit = 0; bit < 8; bit++) {
      uint64_t tmp = b & M(width - 1);
      b = (b << 1) & mask;
      if ((tmp & mask) != 0) {
      	b ^= polynomial & mask;
      }
    }
    t[i] = reversed ? reverse(b, width) : b;
  }
}


uint64_t calc_crc (uint64_t *table, uint8_t *data, size_t size, crc_model *m) {
  uint64_t mask = make_mask(m -> width);
  
  uint64_t crc = m -> init;
  size_t i;
  for (i = 0; i < size; i++) {
    uint8_t b = data[i];
    if (m -> refl_in) {
      b = reverse(b, 8);
    }
    crc = (crc ^ (b << (m -> width - 8)));
    uint8_t ind = (crc >> (m -> width - 8));
    crc = (crc << 8) & mask;
    crc = (crc ^ table[ind]) & mask;
  }
  if (m -> refl_out) {
    crc = reverse(crc, m -> width);
  }
  return (crc ^ m -> xor_out) & mask;

}
