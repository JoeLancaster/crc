#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "crc.h"
#include "util.h"
#include "model.h"

#define TOK_SZ 64
#define MAX_WIDTH 64
#define WIDTH 1
#define REFLIN 2
#define REFLOUT 4
#define POLY 8
#define INIT 16
#define XOROUT 32
#define NAME 64
#define ALL (WIDTH | REFLIN | REFLOUT | POLY | INIT | XOROUT | NAME)

#define BSTR(X) ((X) ? "True" : "False")

extern int errno;

int match_model (char * name) {
  //if no match ret -1
  return (int)name[0];//suppress warnings while WIP
}

int parse_bool (char *str) {
  if (!strcmp(str, "TRUE")) {
    return 1;
  }
  if (!strcmp(str, "FALSE")) {
    return 0;
  }
  return -1;
}

void print_model (crc_model *m, int v) {
  char *s;
  if (v) {
    s = "Name: %s\n"
      "Width (bits): %d\n"
      "Polynomial: 0x%X\n"
      "Initial Value: 0x%X\n"
      "Final Xor: 0x%X\n"
      "Reflected Input: %s\n"
      "Reflected Output %s\n";
  } else {
    s = "name:%s, "
      "width:%d, "
      "poly:%d, "
      "init:%d, "
      "xorout:%d, "
      "reflin:%s, "
      "reflout:%s\n";
  }
  printf(s, "Name", m -> width, m -> poly,
	 m -> init, m -> xor_out,
	 BSTR(m -> refl_in), BSTR(m -> refl_out));
}



void parse_model(char *str, crc_model *m) { //str is one crc model in text format
  char *token;
  int sat = 0;
  token = strtok(str, "\n");
  while (token != NULL) {
    strupper(token);
    if (strcmp(token, "") != 0) { //ignore empty lines
      int r = match_tok(token, m);
      if (r < 0) {
	fprintf(stderr, "Error parsing line: %s\n", token);
	exit(EXIT_FAILURE);
      }
      sat |= r;
    }
    token = strtok(NULL, "\n");
  }
  if (sat != ALL) {
    fprintf(stderr, "Error when parsing model: a field is missing.\n");
    exit(EXIT_FAILURE);
  }
  if (m -> width == 0) {
    fprintf(stderr, "Error: model width is zero.\n");
    exit(EXIT_FAILURE);
  }
  const uint64_t width_max = make_mask(m -> width);
  if (m -> poly > width_max) {
    fprintf(stderr, "Error: Polynomial is too large to fit in %d bits\n", m -> width);
    exit(EXIT_FAILURE);
  }
  if (m -> init > width_max) {
    fprintf(stderr, "Error: Initial value is too large to fit in %d bits\n", m -> width);
    exit(EXIT_FAILURE);
  }
  if (m -> xor_out > width_max) {
    fprintf(stderr, "Error: Xor out value is too large to fit in %d bits\n", m -> width);
    exit(EXIT_FAILURE);
  }
}

//expects everything to be uppercase
int match_tok (char *str, crc_model *m) {
  char * endptr;
  char key[TOK_SZ];
  char val[TOK_SZ];
  int i = 0;
  int j = 0;
  while (str[i] != ':') {
    key[i] = str[i];
    i++;
  }
  key[i] = '\0';
  j = i + 1;
  i = 0;
  while (str[j] != '\0')  { //val starts far after : 
    val[i] = str[j];
    i++;
    j++;
  }
  val[i] = '\0';
  //uppercase
  if (!strcmp(key, "WIDTH")) {
    int width = atoi(val);
    if (width < 1) {
      return -1; //
    }
    m -> width = width;
    return WIDTH;
  } else if (!strcmp(key, "REFLIN")) {
    m -> refl_in = parse_bool(val);
    if (m -> refl_in == -1) {
      return -1; //
    }
    return REFLIN;
  } else if (!strcmp(key, "REFLOUT")) {
    m -> refl_out = parse_bool(val);
    if (m -> refl_out == -1) {
      return -1; //
    }
    return REFLOUT;
    //check for correct width when parsing these
    //probably make a function out of parsing hex numbers
  } else if (!strcmp(key, "POLY")) { 
    unsigned long long pnom = strtoull(val, &endptr, 16);
    if (errno != 0 || *endptr != '\0') {
      return -1; //
    }
    m -> poly = (uint64_t)pnom;
    return POLY;    
  } else if (!strcmp(key, "INIT")) {
    unsigned long long init = strtoull(val, &endptr, 16); 
    if (errno != 0 || *endptr != '\0') {
      return -1; //
    }
    m -> init = (uint64_t)init;
    return INIT;
  } else if (!strcmp(key, "XOROUT")) {
    unsigned long long xorout = strtoull(val, &endptr, 16); 
    if (errno != 0 || *endptr != '\0') {
      return -1; //
    }
    m -> xor_out = (uint64_t)xorout;
    return XOROUT;
  } else if (!strcmp(key, "NAME")) {
    if (!strcmp(key, "")) {
      return -1; //empty name
    }
    strcpy(m -> name, val);
    return NAME;
  }
  return -1;
}
