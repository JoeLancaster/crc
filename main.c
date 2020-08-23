#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>

#define M(X) (0x00000001 << (X))

const char *usage = "crc -p X -[r|t]\n"
  "\tWhere X is a 32-bit hexadecimal number.\n"
  "\t-r: Run reflected calculation\n"
  "\t-R: Reflect the input polynomial\n"
  "\t-t: Print the calculated table\n"
  "\t-h: Print this message";

extern int errno;
uint32_t table[256];

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

int main (int argc, char ** argv) {
  int rev_poly = 0;
  int rev_out = 0;
  int opt;
  int p_sat = 0;
  int prnt_tab = 0;

  char *endptr;
  char *str = "";
  
  if (argc < 2) {
    puts(usage);
    exit(EXIT_FAILURE);
  }
  while ((opt = getopt(argc, argv, "hrtRp:")) != -1) {
    switch (opt) {
    case 't':
      prnt_tab = 1;
      break;
    case 'h':
      puts(usage);
      return 0;
    case 'r':
      rev_out = 1;
      break;
    case 'R':
      rev_poly = 1;
      break;
    case 'p':
      str = optarg;
      p_sat = 1;
      break;
    case '?':
      if (optopt == 'p') {
	fprintf(stderr, "Option -p requires a polynomial argument.\n");
      }
      else if (isprint(optopt)) {
	fprintf(stderr, "Unkown option \'-%c\'.\nUse ./crc -h for help.\n", optopt);
      }
      exit(EXIT_FAILURE);
    default:
      abort();
    }    
  }
  if (!p_sat) {
    fprintf(stderr, "Option -p is required.\n");
    exit(EXIT_FAILURE);
  }
  errno = 0; 
  unsigned long long pnom = strtoull(str, &endptr, 16);
  if (errno != 0 || *endptr != '\0') {
    fprintf(stderr, "Expected a 32-bit hex number\n");
    exit(EXIT_FAILURE);
  }
  uint32_t _pnom = pnom;
  if (rev_poly) {
    _pnom = reverse(_pnom);
    printf("Polynomial as reflected: 0x%08X\n", _pnom);
  }
  gen_table(table, _pnom, rev_out);
  if (prnt_tab) {
    int i;
    for (i = 0; i < 256; i++) {
      if (i % 4 == 0)  {
	putchar('\n');
      }
      printf("0x%08X, ", table[i]);
    }
  } else {
    
  }
  return 0;
}
