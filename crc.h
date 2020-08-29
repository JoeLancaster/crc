#ifndef CRC_G_H
#define CRC_G_H

#include <stdint.h>
#include <stddef.h>

#include "model.h"

uint64_t reverse (uint64_t,  int);
//reverse8
void gen_table (uint64_t *, int, crc_model *);
uint64_t calc_crc (uint64_t *, uint8_t *, size_t, crc_model *);
uint64_t make_mask (int);

#endif
