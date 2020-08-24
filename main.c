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

#define READ_MAX 4096

const char *usage = "crc -p X -[r|t]\n"
  "\tWhere X is a 32-bit hexadecimal number.\n"
  "\t-r: Run reflected calculation\n"
  "\t-R: Reflect the input polynomial\n"
  "\t-t: Just print the calculated table\n"
  "\t-m: Use a standard CRC model\n"
  "\t-w: Define the width of the polynomial\n"
  "\t-h: Print this message";

extern int errno;
uint32_t table[256];

int main (int argc, char ** argv) {
  int rev_poly = 0; //flags
  int rev_out = 0;
  int opt;
  int p_sat = 0;
  int m_sat = 0;  
  int prnt_tab = 0;
  char *str = "";
  char *m_str = "";
  char *w_str = "";
  uint32_t width;
  
  char *endptr;
  
  FILE *fd;
  int64_t file_size;
  char * rd_buf;
    
  if (argc < 2) {
    puts(usage);
    exit(EXIT_FAILURE);
  }
  
  while ((opt = getopt(argc, argv, "m:w:hrtRp:")) != -1) {
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
    case 'm':
      m_str = optarg;
      m_sat = 1;
      break;
    case 'w':
      w_str = optarg;
      break;
    case '?':
      if (optopt == 'p' || optopt == 'm' || optopt == 'w') {
	fprintf(stderr, "Option -%c requires a polynomial argument.\n", optopt);
      }
      else if (isprint(optopt)) {
	fprintf(stderr, "Unkown option \'-%c\'.\nUse ./crc -h for help.\n", optopt);
      }
      exit(EXIT_FAILURE);
    default:
      abort();
    }    
  }
  if ( (p_sat && m_sat) || !(p_sat || m_sat) ) {
    fprintf(stderr, "Either -p or -m is required.\n");
    exit(EXIT_FAILURE);
  }
  errno = 0;
  unsigned long long pnom = strtoull(str, &endptr, 16);
  if (errno != 0 || *endptr != '\0') { // != '\0' require a stricter parse
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
    if (optind == argc) {
      fprintf(stderr, "No file location provided!\n");
      exit(EXIT_FAILURE);
    }
    char *path = malloc(PATH_MAX);
    errno = 0;
    realpath(argv[optind], path);
    if (errno != 0) {
      fprintf(stderr, "Error finding file. realpath: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    struct stat st;
    errno = 0;
    if (stat(path, &st) == 0) { //can set errno but quit later
      file_size = st.st_size;
    }
    if (errno == 0 && file_size == 0) {
      fprintf(stderr, "Error: Empty file.");
      exit(EXIT_FAILURE);
    }
    fd = fopen(path, "rb"); //+ text mode?
    if (errno != 0) {
      fprintf(stderr, "Error opening file. fopen: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    fclose(fd);
  }
  return 0;
}
