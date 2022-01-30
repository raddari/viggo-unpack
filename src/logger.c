#include "logger.h"


static int current_log_level = LOG_LEVEL_INFO;


int log_get_level() {
  return current_log_level;
}

void log_set_level(int log_level) {
  current_log_level = log_level;
}
