#include "sfx.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// FIXME(raddari): `arpa/inet.h` is not portable (#1)
#include <arpa/inet.h>

#define READ_N(file, target, n)                                      \
    {                                                                \
      size_t nread = fread((target), sizeof *(target), (n), (file)); \
      if (nread < (n)) {                                             \
        fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);              \
        if (feof(file)) {                                            \
          fprintf(stderr, "End of file reading " #target "\n");      \
        }                                                            \
        if (ferror(file)) {                                          \
          fprintf(stderr, "Error reading " #target "\n");            \
        }                                                            \
      }                                                              \
    }

#define READ_ARRAY(file, target, n) READ_N(file, target, n)

#define READ_ONE(file, target) READ_N(file, &target, 1)

// FIXME(raddari): not portable (#2)
#define READ_BE(file, target) \
    {                         \
      READ_ONE(file, target); \
      target = htonl(target); \
    }

// FIXME(raddari): not portable (#2)
#define READ_LE(file, target) READ_ONE(file, target)


static AifHeader parse_vagp_header(FILE *file);
static AifBlock* parse_aif_block(FILE *file);

static const u8 SFX_MAGIC[4] = {0x46, 0x46, 0x46, 0x58};
static const u8 BLOCK_END[16] = {
    0x07, 0x00, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
};


VagpAudio** sfx_parse_container(FILE *sfx_file) {
  if (!sfx_file) {
    return NULL;
  }

  fseek(sfx_file, 0, SEEK_SET);
  u8 buffer[4];
  READ_ARRAY(sfx_file, buffer, sizeof buffer);

  if (memcmp(SFX_MAGIC, buffer, sizeof buffer) != 0) {
    u32 buffer_size = sizeof buffer;
    fprintf(stderr, "Malformed magic bytes. Expected: ");
    for (u32 i = 0; i < buffer_size; i++) {
      fprintf(stderr, "0x%.2X ", SFX_MAGIC[i]);
    }
    fprintf(stderr, "; Got: ");
    for (u32 i = 0; i < buffer_size; i++) {
      fprintf(stderr, "0x%.2X", buffer[i]);
    }
    fprintf(stderr, "\n");
    return NULL;
  }

  u32 elements;
  READ_LE(sfx_file, elements);
  VagpAudio** container = malloc((elements + 1) * sizeof *container);

  u32 *vagp_info = malloc(2 * elements * sizeof *vagp_info);
  for (u32 i = 0; i < 2 * elements; i += 2) {
    READ_LE(sfx_file, vagp_info[i]);
    READ_LE(sfx_file, vagp_info[i + 1]);
  }

  fprintf(stderr, "Discovered %u addresses at:\n", elements);
  for (u32 i = 0; i < 2 * elements; i += 2) {
    fprintf(stderr, "addr=0x%.4X, size=0x%.4X\n", vagp_info[i], vagp_info[i + 1]);
  }

  // TODO(raddari): find a use for the info table
  free(vagp_info);
  vagp_info = NULL;

  for (u32 i = 0; i < elements; i++) {
    VagpAudio *audio = malloc(sizeof *audio);
    audio->header = parse_vagp_header(sfx_file);

    AifBlock *block = parse_aif_block(sfx_file);
    audio->first = block;

    while (memcmp(BLOCK_END, block, sizeof *block) != 0) {
      AifBlock *next = parse_aif_block(sfx_file);
      block->next = next;
      block = next;
    }
    container[i] = audio;
  }
  container[elements] = NULL;
  return container;
}

void sfx_container_destroy(VagpAudio **container) {
  if (!container) {
    return;
  }

  for (int i = 0; container[i] != NULL; i++) {
    AifBlock *block = container[i]->first;
    while (block) {
      AifBlock *next = block->next;
      free(block);
      block = next;
    }
    free(container[i]);
    container[i] = NULL;
  }
  free(container);
}

static AifHeader parse_vagp_header(FILE *file) {
  AifHeader header = {0};

  READ_ARRAY(file, header.magic, 4);
  READ_BE(file, header.version);
  READ_BE(file, header.offset);
  READ_BE(file, header.size);
  READ_BE(file, header.rate);
  READ_ARRAY(file, header._r0, 10);
  READ_BE(file, header.channels);
  READ_BE(file, header._r1);
  READ_ARRAY(file, header.title, 32);

  return header;
}

static AifBlock* parse_aif_block(FILE *file) {
  AifBlock *block = malloc(sizeof *block);

  READ_LE(file, block->coefficient);
  READ_LE(file, block->loop);
  READ_ARRAY(file, block->data, 14);

  return block;
}
