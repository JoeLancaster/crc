#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

#define MOD_NAME_SZ 32

typedef struct {
  int width;
  int refl_in;
  int refl_out;
  uint64_t poly;
  uint64_t init;
  uint64_t xor_out;
  char name[MOD_NAME_SZ];
} crc_model;

int match_model (char *);
void print_model (crc_model *, int);
int parse_bool (char *);
int match_tok (char *, crc_model *);
void parse_model (char *, crc_model *);

#endif
