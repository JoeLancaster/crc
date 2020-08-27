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

const char *usage = "usage: crc -[m|M] [OPTIONS]\n"
  "\t-m: Use a standard CRC model. See -h models for a list of models\n"
  "\t-M: Use a user-defined model from a file\n"
  "\n"
  "\t-t: Just print the calculated table\n"
  "\t-r: Use with -t. Print the reflected table\n"
  "\t-h: Print this message";

extern int errno;
uint64_t table[256];

int main (int argc, char ** argv) {
  crc_model m = {
    .width = 32,
    .refl_in = 1,
    .refl_out = 1,
    .poly = 0x4C11DB7U,
    .init = 0xFFFFFFFF,
    .xor_out = 0xFFFFFFFF
    //.name 
  };
  
  int rev_out = 0;
  int opt;
  int m_sat = 0;
  int M_sat = 0;
  int prnt_tab = 0;
  char *m_str = "";
  char *M_str = "";

  uint8_t * rd_buf;
  uint8_t * model_buf;
    
  if (argc < 2) {
    puts(usage);
    exit(EXIT_FAILURE);
  }
  
  while ((opt = getopt(argc, argv, "M:m:hrt")) != -1) {
    switch (opt) {
    case 't':
      prnt_tab = 1;
      break;
    case 'h':
      puts(usage);
      return 0;
    case 'r':
      rev_out = 1; //reflect output table
      break;
    case 'm':
      m_str = optarg;
      m_sat = 1;
      break;
    case 'M':
      M_str = optarg;
      M_sat = 1;
      break;
    case '?':
      if (optopt == 'm') {
	fprintf(stderr, "-m requires a model argument. For a list of models see -h models.");
      } else if (optopt == 'M') {
	fprintf(stderr, "-M requires a path to a model file. For help with syntax see -h syntax");
      }
      exit(EXIT_FAILURE);
      break;
    default:
      abort();
    }    
  }
  if ( (M_sat && m_sat) || !(M_sat || m_sat) ) {
    fprintf(stderr, "Either -M or -m is required.\n");
    exit(EXIT_FAILURE);
  }
  if (M_sat) {
    dump_file(M_str, &model_buf, READ_MAX);

    parse_model((char *)model_buf, &m);
   

    free(model_buf);
  } else if (m_sat) {
    //use m
  }
  gen_table(table, rev_out, &m);
  print_model(&m, 1);
  if (prnt_tab) {
    int i;
    for (i = 0; i < 256; i++) {
      if (i % 4 == 0)  {
	putchar('\n');
      }
      print_hex(m.width, table[i]);
      printf(", ");
    }
  } else {
    if (optind == argc) {
      fprintf(stderr, "No file location provided!\n");
      exit(EXIT_FAILURE);
    }

    size_t read = dump_file(argv[optind], &rd_buf, READ_MAX);
    printf("read: %lu\n", read);
    uint32_t crc = calc_crc(table, rd_buf, read, &m);
    
    printf("0x%08X\n", crc);

  }
  return 0;
}
