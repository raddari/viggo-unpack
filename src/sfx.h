#pragma once

#include "types.h"

#include <stdio.h>


typedef struct AifHeader {
  char magic[4];
  u32 version;
  u32 offset;
  u16 flags;
  u8 _r0[2];
  u32 rate;
  u8 _r1[10];
  u8 channels;
  u8 _r2[1];
  char title[32];
} AifHeader;

typedef struct AifBlock {
  u8 coefficient;
  u8 loop;
  u8 data[14];
  struct AifBlock *next;
} AifBlock;

typedef struct VagpAudio {
  AifHeader *header;
  AifBlock *first;
} VagpAudio;

typedef enum {
  AifFlag0 = 1u << 0,
  AifFlag1 = 1u << 1,
  AifFlag2 = 1u << 2,
  AifFlag3 = 1u << 3,
  AifFlag4 = 1u << 4,
  AifFlag5 = 1u << 5,
  AifFlag6 = 1u << 6,
  AifFlag7 = 1u << 7,
  AifFlag8 = 1u << 8,
  AifFlag9 = 1u << 9,
  AifFlagA = 1u << 10,
  AifFlagB = 1u << 11,
  AifFlagC = 1u << 12,
  AifFlagD = 1u << 13,
  AifFlagE = 1u << 14,
  AifFlagF = 1u << 15,
} AifFlags;


/// Produces a NULL-terminated array of `VagpAudio` structs
/// decoded from the given SFX file. This function may return
/// NULL if an unrecoverable error occurs during parsing.
///
/// The container may be destroyed, along with all the audio
/// files inside with a call to `sfx_container_destroy`.
VagpAudio** sfx_parse_container(FILE *sfx_file);
void sfx_container_destroy(VagpAudio **container);
