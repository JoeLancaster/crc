#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

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

extern int errno;

int parse_bool (char *str) {
  if (!strcmp(str, "TRUE")) {
    return 1;
  }
  if (!strcmp(str, "FALSE")) {
    return 0;
  }
  return -1;
}

void parse_model(char *str, crc_model *m) { //str is one crc model in text format
  char *token;
  int sat = 0;
  token = strtok(str, "\n");
  while (token != NULL) {
    int i;
    for (i = 0; i < strlen(token); i++) {
      token[i] = toupper(token[i]);
    }
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
    fprintf(stderr, "Error when parsing model: a field is missing.\nsat: %d all: %d\n", sat, ALL);
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
  int off = 0;
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
