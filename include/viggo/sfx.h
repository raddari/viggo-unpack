#pragma once

#include "types.h"

#include <stdio.h>


typedef struct AifHeader {
  char magic[4];
  u32 version;
  u32 offset;
  u32 size;
  u32 rate;
  u8 _r0[10];
  u8 channels;
  u8 _r1;
  char title[32];
} AifHeader;


AifHeader aif_header_from(FILE *file);
