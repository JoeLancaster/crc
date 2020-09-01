#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>

#include "util.h"

void print_hex (int width, uint64_t number) {
  int leadingz = width / 4 + (width % 4 != 0);
  printf("0x%0*" PRIX64, leadingz, number);
}

size_t dump_file (char *arg_path, uint8_t **buf, const size_t READ_MAX) {
  struct stat st;
  char * path = malloc(PATH_MAX);
  size_t file_size = 0;
  FILE * fd;
  errno = 0;
  realpath(arg_path, path);
  if (errno != 0) {
    fprintf(stderr, "Error finding file %s. (realpath): %s\n", arg_path, strerror(errno));
    exit(EXIT_FAILURE);
  }
  errno = 0;
  if (stat(path, &st) == 0) { //can set errno but quit later
    file_size = st.st_size;
  }
  if (errno == 0 && file_size == 0) {
    fprintf(stderr, "Error: Empty file.");
    exit(EXIT_FAILURE);
  }
  fd = fopen(path, "rb");
  if (errno != 0) {
    fprintf(stderr, "Error opening file. fopen: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  size_t buf_sz = file_size < READ_MAX ? file_size : READ_MAX;
  *buf = calloc(sizeof(char), buf_sz + 1); //let the buffer be zeroed and +1 so the buffer (now string) is null-terminated
  if (!(*buf)) {
    fprintf(stderr, "malloc error\n");
    exit(EXIT_FAILURE);
  }
  size_t read = fread(*buf, 1, buf_sz, fd);
  if (read != buf_sz) {
    fprintf(stderr, "Read: %zu/%zu bytes.\n", read, buf_sz);
    exit(EXIT_FAILURE);
  }
  fclose(fd);
  free(path);
  return read;
}

void strupper (char * s) {
  size_t i;
  for (i = 0; i < strlen(s); i++) {
    s[i] = toupper(s[i]);
  }
  return;
}
