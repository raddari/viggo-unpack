#include "sfx.h"

#include "logger.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// FIXME(raddari): `arpa/inet.h` is not portable (#1)
#include <arpa/inet.h>

#define READ_N(file, target, n)                                      \
    {                                                                \
      size_t nread = fread((target), sizeof *(target), (n), (file)); \
      if (nread < (n)) {                                             \
        if (feof(file)) {                                            \
          WARN("End of file reading " #target);                      \
        }                                                            \
        if (ferror(file)) {                                          \
          WARN("Error reading " #target);                            \
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


static AifHeader* parse_vagp_header(FILE *file);
static AifBlock* parse_aif_block(FILE *file);

static const u8 SFX_MAGIC[4] = {0x46, 0x46, 0x46, 0x58}; // FFFX


VagpAudio** sfx_parse_container(FILE *sfx_file) {
  if (!sfx_file) {
    return NULL;
  }

  fseek(sfx_file, 0, SEEK_SET);
  u8 buffer[4];
  READ_ARRAY(sfx_file, buffer, sizeof buffer);

  if (memcmp(SFX_MAGIC, buffer, sizeof buffer) != 0) {
    u32 buffer_size = sizeof buffer;
    ERROR("Malformed magic bytes. Expected:");
    for (u32 i = 0; i < buffer_size; i++) {
      ERROR("0x%.2X ", SFX_MAGIC[i]);
    }
    ERROR("Got:");
    for (u32 i = 0; i < buffer_size; i++) {
      ERROR("0x%.2X", buffer[i]);
    }
    return NULL;
  }

  struct {
    u32 elements;
    u32 *addresses;
    u32 *sizes;
  } sfx_info;

  READ_LE(sfx_file, sfx_info.elements);
  sfx_info.addresses = malloc(sfx_info.elements * sizeof *sfx_info.addresses);
  sfx_info.sizes = malloc(sfx_info.elements * sizeof *sfx_info.sizes);

  DEBUG("Discovered %u addresses at:", sfx_info.elements);
  for (u32 i = 0; i < sfx_info.elements; i++) {
    READ_LE(sfx_file, sfx_info.addresses[i]);
    READ_LE(sfx_file, sfx_info.sizes[i]);
    DEBUG("addr=0x%.8X; size=0x%.8X", sfx_info.addresses[i], sfx_info.sizes[i]);
  }

  VagpAudio** container = malloc((sfx_info.elements + 1) * sizeof *container);
  for (u32 i = 0; i < sfx_info.elements; i++) {
    VagpAudio *audio = malloc(sizeof *audio);
    audio->header = parse_vagp_header(sfx_file);

    u32 block_size = sfx_info.sizes[i] - sizeof (AifHeader);
    u32 blocks_remaining = (block_size / (sizeof (AifBlock) - sizeof (AifBlock *))) - 1;
    DEBUG("block_size=%u, blocks_remaining=%u", block_size, blocks_remaining);

    AifBlock *block = parse_aif_block(sfx_file);
    audio->first = block;

    while (blocks_remaining-- > 0) {
      AifBlock *next = parse_aif_block(sfx_file);
      block->next = next;
      block = next;
    }
    container[i] = audio;
  }

  free(sfx_info.addresses);
  free(sfx_info.sizes);

  container[sfx_info.elements] = NULL;
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
    free(container[i]->header);
    free(container[i]);
    container[i] = NULL;
  }
  free(container);
}

static AifHeader* parse_vagp_header(FILE *file) {
  AifHeader *header = malloc(sizeof *header);

  READ_ARRAY(file, header->magic, 4);
  READ_BE(file, header->version);
  READ_BE(file, header->offset);
  READ_BE(file, header->size);
  READ_BE(file, header->rate);
  READ_ARRAY(file, header->_r0, 10);
  READ_BE(file, header->channels);
  READ_BE(file, header->_r1);
  READ_ARRAY(file, header->title, 32);

  return header;
}

static AifBlock* parse_aif_block(FILE *file) {
  AifBlock *block = malloc(sizeof *block);
  block->next = NULL;

  READ_LE(file, block->coefficient);
  READ_LE(file, block->loop);
  READ_ARRAY(file, block->data, 14);

  return block;
}
