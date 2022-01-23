#include "sfx.h"

#include <stdio.h>


static void print_header(AifHeader *header);


int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Please supply file path\n");
    return 1;
  }

  FILE *file = fopen(argv[1], "rb");
  if (!file) {
    fprintf(stderr, "Cannot open file\n");
    return 1;
  }

  fseek(file, 0x40, SEEK_SET);
  AifHeader header = aif_header_from(file);
  fclose(file);

  print_header(&header);
  return 0;
}

static void print_header(AifHeader *header) {
  printf("AifHeader{\n"
      "  magic=%s\n"
      "  version=%u\n"
      "  offset=%u\n"
      "  size=%u\n"
      "  rate=%u\n"
      "  channels=%u\n"
      "  title=%s\n"
      "}\n",
      header->magic, header->version, header->offset,
      header->size, header->rate, header->channels,
      header->title);
}
