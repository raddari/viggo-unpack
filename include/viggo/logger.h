#pragma once

#include <stdio.h>


#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARN  1
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_TRACE 4

#ifndef LOG_SILENCE
  #define LOG_FILE(file, prefix, level, msg, ...)                                         \
      {                                                                                   \
        if (log_get_level() >= level) {                                                   \
          fprintf(file, prefix " (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        }                                                                                 \
      }
#else
  #define LOG_FILE(file, prefix, level, msg, ...)
#endif

#define LOG_MESSAGE(prefix, level, msg, ...) LOG_FILE(stderr, prefix, level, msg, ##__VA_ARGS__)

#define ERROR(msg, ...) LOG_MESSAGE("[ERROR]", LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
#define WARN(msg, ...)  LOG_MESSAGE("[WARN]", LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#define INFO(msg, ...)  LOG_MESSAGE("[INFO]", LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#define DEBUG(msg, ...) LOG_MESSAGE("[DEBUG]", LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#define TRACE(msg, ...) LOG_MESSAGE("[TRACE]", LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)


int log_get_level();
void log_set_level(int log_level);
