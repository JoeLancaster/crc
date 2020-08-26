#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "crc.h"
#include "util.h"
#include "model.h"

#define READ_MAX 4096

const char *usage = "crc -p X -[r|t]\n"
  "\tWhere X is a 32-bit hexadecimal number.\n"
  "\t-r: Run reflected calculation\n"
  "\t-R: Reflect the input polynomial\n"
  "\t-t: Just print the calculated table\n"
  "\t-m: Use a standard CRC model\n"
  "\t-M: Use a user-defined model from a file\n"
  "\t-w: Define the width of the polynomial\n"
  "\t-h: Print this message";

extern int errno;
uint32_t table[256];

int main (int argc, char ** argv) {
  crc_model m = {
    .width = 32,
    .refl_in = 1,
    .refl_out = 1,
    .poly = 0x4C11DB7U,
    .init = 0xFFFFFFFF,
    .xor_out = 0xFFFFFFFF
  };
  
  int rev_poly = 0; //flags
  int rev_out = 0;
  int opt;
  int p_sat = 0;
  int m_sat = 0;
  int M_sat = 0;
  int prnt_tab = 0;
  char *str = "";
  char *m_str = "";
  char *M_str = "";
  char *w_str = "";
  uint32_t width;
  
  char *endptr;
  uint32_t _pnom;

  char * rd_buf;
  char * model_buf;
    
  if (argc < 2) {
    puts(usage);
    exit(EXIT_FAILURE);
  }
  
  while ((opt = getopt(argc, argv, "M:m:w:hrtRp:")) != -1) {
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
      rev_poly = 1; //probably remove this, it's confusing
      break;
    case 'p': //and this, just use model
      str = optarg;
      p_sat = 1;
      break;
    case 'm':
      m_str = optarg;
      m_sat = 1;
      break;
    case 'M':
      M_str = optarg;
      M_sat = 1;
    case 'w':
      w_str = optarg;
      break;
    case '?':
      if (optopt == 'p' || optopt == 'm' || optopt == 'w') {
	fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      }
      else if (isprint(optopt)) {
	fprintf(stderr, "Unkown option \'-%c\'.\nUse ./crc -h for help.\n", optopt);
      }
      exit(EXIT_FAILURE);
    default:
      abort();
    }    
  }
  if ( (M_sat && m_sat) || !(M_sat || m_sat) ) {
    fprintf(stderr, "Either -M or -m is required.\n");
    exit(EXIT_FAILURE);
  }
  if (M_sat) {
    //open model file and parse
    dump_file(M_str, &model_buf, READ_MAX);

    parse_model(model_buf, &m);
    
    _pnom = m.poly; //change

    free(model_buf);
  } else if (m_sat) {
    _pnom = m.poly;
  }
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
    if (optind == argc) {
      fprintf(stderr, "No file location provided!\n");
      exit(EXIT_FAILURE);
    }

    size_t read = dump_file(argv[optind], &rd_buf, READ_MAX);
    printf("read: %lu\n", read);
    uint32_t crc = calc_crc(table, _pnom, rd_buf, read, &m);
    
    printf("0x%08X\n", crc);

  }
  return 0;
}
