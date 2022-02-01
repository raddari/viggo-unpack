#include "logger.h"
#include "sfx.h"

#include <errno.h>
#include <stdio.h>


static void print_header(AifHeader *header);


int main(int argc, char *argv[]) {
  logger_set_level(LOG_LEVEL_TRACE);

  if (argc != 2) {
    fprintf(stderr, "Please supply file path\n");
    return 1;
  }

  FILE *file = fopen(argv[1], "rb");
  if (!file) {
    perror("Error opening file");
    return 1;
  }

  VagpAudio** container = sfx_parse_container(file);
  for (int i = 0; container[i] != NULL; i++) {
    VagpAudio* audio = container[i];
    print_header(audio->header);
    printf("\n");
  }
  sfx_container_destroy(container);
  container = NULL;

  fclose(file);
  return 0;
}

static void print_header(AifHeader *header) {
  if (!header) {
    return;
  }

  printf("AifHeader {\n"
      "  magic=%s\n"
      "  version=%u\n"
      "  offset=%u\n"
      "  flags=%u\n"
      "  rate=%u\n"
      "  channels=%u\n"
      "  title=%s\n"
      "}\n",
      header->magic, header->version, header->offset,
      header->flags, header->rate, header->channels,
      header->title);
}
