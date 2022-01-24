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

typedef struct AifBlock {
  u8 coefficient;
  u8 loop;
  u8 data[14];
  struct AifBlock *next;
} AifBlock;

typedef struct VagpAudio {
  AifHeader header;
  AifBlock *first;
} VagpAudio;


/// Produces a NULL-terminated array of `VagpAudio` structs
/// decoded from the given SFX file. This function may return
/// NULL if an unrecoverable error occurs during parsing.
///
/// The container may be destroyed, along with all the audio
/// files inside with a call to `sfx_container_destroy`.
VagpAudio** sfx_parse_container(FILE *sfx_file);
void sfx_container_destroy(VagpAudio **container);
