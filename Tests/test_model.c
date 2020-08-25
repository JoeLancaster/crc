#include <stdio.h>
#include <string.h>

#include "../model.h"

char * str1 = "WIDTH:32";
char * str2 = "WIDTH:-32";

char * str3 = "NAME:CRC-32";

char * str4 = "REFLIN:TRUE";
char * str5 = "REFLIN:FALSE";
char * str6 = "REFLOUT:TRUE";

char * str7 = "POLY:0xFFFFFFFF";
char * str8 = "INIT:0x00FF00FF";
char * str9 = "XOROUT:0x00000011";

char model_str[] = "width:32\n"
  "reflin:true\n"
  "reflout:true\n"
  "poly:0xFFFFFFFF\n"
  "init:0xFFFFFFFF\n"
  "xorout:0xFFFFFFFF\n"
  "name:CRC-32\n\n\n\n"; //extra newlines

int main () {
  crc_model m;
  int r;
  printf("Testing match_tok: \n");
  if ( (r = match_tok(str1, &m)) < 0 || m.width != 32 ) {
    fprintf(stderr, "(Fail) match_tok returns %d with arg %s\n", r, str1);
  } else {
    printf("(Pass) m.width: %lu\n", m.width);
  }
  if ( (r = match_tok(str2, &m)) < 0 ) {
    printf("(Pass) match_tok returns %d with arg %s\n", r, str2);
  } else {
    fprintf(stderr, "(Fail) m.width: %lu\n", m.width);
  }
  if ( (r = match_tok(str3, &m)) < 0 || strcmp(m.name, "CRC-32") != 0) {
    fprintf(stderr, "(Fail) match_tok returns %d with arg %s\n", r, str3);
  } else {
    printf("(Pass) m.name: %s\n", m.name);
  }
  if ( (r = match_tok(str4, &m)) < 0 || m.refl_in != 1) {
    fprintf(stderr, "(Fail) match_tok returns %d with arg %s\n", r, str4);
  } else {
    printf("(Pass) m.refl_in: %d\n", m.refl_in);
  }
  if ( (r = match_tok(str5, &m)) < 0 || m.refl_in != 0) {
    fprintf(stderr, "(Fail) match_tok returns %d with arg %s\n", r, str5);
  } else {
    printf("(Pass) m.refl_in: %d\n", m.refl_in);
  }
  if ( (r = match_tok(str6, &m)) < 0 || m.refl_out != 1) {
    fprintf(stderr, "(Fail) match_tok returns %d with arg %s\n", r, str6);
  } else {
    printf("(Pass) m.refl_out: %d\n", m.refl_out);
  }
  if ( (r = match_tok(str7, &m)) < 0 || m.poly != 0xFFFFFFFFU) {
    fprintf(stderr, "(Fail) match_tok returns %d with arg %s", r, str7);
  } else {
    printf("(Pass) m.poly: 0x%08X\n", m.poly);
  }
  if ( (r = match_tok(str8, &m)) < 0 || m.init != 0x00FF00FFU) {
    fprintf(stderr, "(Fail) match_tok returns %d with arg %s\n", r, str8);
  } else {
    printf("(Pass) m.init: 0x%08X\n", m.init);
  }
  if ( (r = match_tok(str9, &m)) < 0 || m.xor_out != 0x000000011U) {
    fprintf(stderr, "(Fail) match_tok returns %d with arg %s\n", r, str9);
  } else {
    printf("(Pass) m.xor_out: 0x%08X\n", m.xor_out);
  }
  printf("Testing parse_model:\n");
  crc_model n;
  parse_model(model_str, &n);
  printf("width: %d\n"
	 "refl_in: %d\n"
	 "refl_out: %d\n"
	 "poly: 0x%08X\n"
	 "init: 0x%08X\n"
	 "xor_out: 0x%08X\n"
	 "name: %s\n", n.width, n.refl_in, n.refl_out, n.poly, n.init, n.xor_out, n.name);
  puts("Model passed");
}
