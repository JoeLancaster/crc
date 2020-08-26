#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>

#include "util.h"

size_t dump_file (char *arg_path, char **buf, const size_t READ_MAX) {
  struct stat st;
  char * path = malloc(PATH_MAX);
  int64_t file_size = 0;
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
  *buf = malloc(buf_sz);
  size_t read = fread(*buf, 1, buf_sz, fd);
  if (read != buf_sz) {
    fprintf(stderr, "Read: %d/%d bytes.\n", read, buf_sz);
    exit(EXIT_FAILURE);
  }
  fclose(fd);
  return read;
}
