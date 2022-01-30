#include "logger.h"


static int current_log_level = LOG_LEVEL_INFO;


int logger_get_level() {
  return current_log_level;
}

void logger_set_level(int log_level) {
  current_log_level = log_level;
}
