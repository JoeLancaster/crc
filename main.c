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

const char *usage = "usage: crc -m [OPTIONS]\n"
  "\t-m: Specify a model through either a file or the name of a built-in model\n"
  "\t    See -h models for a list of models or -h syntax for help creating a model file\n"
  "\t-t: Just print the calculated table\n"
  "\t-r: Use with -t. Print the reflected table\n"
  "\t-h: Print this message";

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
  int prnt_tab = 0;
  char *m_str = "";

  uint8_t * rd_buf;
  uint8_t * model_buf;

  char * path;
  
  if (argc < 2) {
    puts(usage);
    exit(EXIT_FAILURE);
  }
  
  while ((opt = getopt(argc, argv, "m:hrt")) != -1) {
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
    case '?':
      if (optopt == 'm') {
	fprintf(stderr, "-m requires either a model file or the name of a built-in model. For a list of models see -h models.");
      exit(EXIT_FAILURE);
      }
      break;
    default:
      abort();
    }    
  }
  if (!m_sat) {
    fprintf(stderr, "Option -m is required.\n");
    exit(EXIT_FAILURE);
  }

  path = malloc (PATH_MAX);
  errno = 0;
  realpath(m_str, path);
  int eval = errno;
  if (eval == 0) {
    dump_file(path, &model_buf, READ_MAX);
    parse_model((char *)model_buf, &m);
    free(model_buf);
  } else if (eval != ENOENT) { //file exists but another error happened
    fprintf(stderr, "Error finding file: %s. realpath: %s\n", m_str, strerror(eval));
    exit(EXIT_FAILURE);
  } else { //file doesn't exist, check for matching model name or number
    char model_name[strlen(m_str)];
    int match;
    strcpy(model_name, m_str);
    strupper(model_name);
    if ( (match = match_model(model_name)) != -1 ) {
      //m = models[match];
    } else {
      fprintf(stderr, "%s is neither a file or a built-in model.\n", m_str);
      exit(EXIT_FAILURE);
    }
  }
  
  gen_table(table, (prnt_tab && rev_out), &m); //dont use reversed table in calculations

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
    uint32_t crc = calc_crc(table, rd_buf, read, &m);
    
    print_hex(m.width, crc);

  }
  return 0;
}
