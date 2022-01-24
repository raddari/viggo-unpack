#include "sfx.h"

// TODO(raddari): `arpa/inet.h` is not portable
#include <arpa/inet.h>

#define READ_N(file, target, n)                                      \
    {                                                                \
      size_t nread = fread((target), sizeof *(target), (n), (file)); \
      if (nread < (n) * sizeof *(target)) {                          \
        if (feof(file)) {                                            \
          fprintf(stderr, "End of file reading " #target "\n");      \
        }                                                            \
        if (ferror(file)) {                                          \
          fprintf(stderr, "Error reading " #target "\n");            \
        }                                                            \
      }                                                              \
    }

#define READ_ARRAY(file, target, n) READ_N(file, target, n)

#define READ_ONE(file, target) READ_N(file, target, 1)

#define READ_FIELD_BE(file, target) \
    {                               \
      READ_ONE(file, &target);      \
      target = ntohl(target);       \
    }

#define READ_FIELD(file, target) READ_FIELD_BE(file, target)


AifHeader aif_header_from(FILE *file) {
  AifHeader header = {0};

  READ_ARRAY(file, header.magic, 4);
  READ_FIELD(file, header.version);
  READ_FIELD(file, header.offset);
  READ_FIELD(file, header.size);
  READ_FIELD(file, header.rate);
  READ_ARRAY(file, header._r0, 10);
  READ_FIELD(file, header.channels);
  READ_FIELD(file, header._r1);
  READ_ARRAY(file, header.title, 32);

  return header;
}
