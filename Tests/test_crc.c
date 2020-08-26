#include <stdint.h>
#include <stdio.h>

#include "../crc.h"

#define M(X) (0x00000001 << (X))

#define bound UINT32_MAX >> 10

uint32_t o_reverse(uint32_t n) {
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

void o_gen_table(uint32_t *t, uint32_t polynomial, int reversed) {
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
    t[i] = reversed ? o_reverse(b) : b;
  }
}

uint32_t o_calc_crc(uint32_t *table, uint32_t polynomial, uint8_t *data, size_t size, crc_model * m) {
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
    crc = o_reverse(crc);
  }
  return crc ^ m -> xor_out;
}


int main () {
  int i;
  uint32_t o;
  uint64_t n;

  crc_model m = {
		 .width = 32,
		 .refl_in = 1,
		 .refl_out = 1,
		 .poly = 0x4C11DB7U,
		 .init = 0xFFFFFFFF,
		 .xor_out = 0xFFFFFFFF
  };
  

  puts("Testing reverse");
  for (o = 0, n = 0; o < bound; n++, o++) {
    if (n > 4000000000 && n % 100000) {
      printf("n: %lu\n", n);
    }
    uint32_t ro = o_reverse(o);
    uint32_t rn = (uint32_t)reverse(n, 32);
    if (rn != ro) {
      fprintf(stderr, "(Fail) reverse doesn't match:\n\t"
	      "o: %lu, n: %lu\n\t"
	      "o_reverse(o): %lu, reverse(n, 32): %lu\n"
	      ,o, n, ro, rn);
      return -1;
    }
  }
  puts("(Pass) ");

  puts("Testing gen_table");

  uint64_t table[256];
  uint32_t o_table[256];
  
  gen_table(table, 1, &m);
  o_gen_table(o_table, 0x04C11DB7, 1);

  

  int pass = 1;
  
  for (i = 0; i < 256; i++) {
    printf("old: 0x%016X\tnew: 0x%016X\n", o_table[i], table[i]);
    if ((uint32_t)table[i] != o_table[i]) {
      puts("(Fail)");
      return -1;
    }
  }
  puts("(Pass)");

  puts("Testing calc_crc:\n");
  char data[] = "123456789";



  
  uint64_t n_result = calc_crc(table, data, 9, &m);
  uint32_t o_result = o_calc_crc(o_table, 0x04C11DB7, data, 9, &m);

  puts(((uint32_t)n_result == o_result) ? "(Pass)" : "(Fail)");
  printf("new method crc: %lu\nold method crc: %lu\n", n_result, o_result);
}
