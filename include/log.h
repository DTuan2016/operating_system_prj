#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ANSI colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

/* Log Level */
typedef enum{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_SUCCESS,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_NONE
}log_level_t;

/* Core API */
void log_init(const char *filename);
void log_close(void);

void log_set_level(log_level_t level);
log_level_t log_get_level(void);

void log_message(log_level_t level , const char *color, const char *fmt, ...);

/* Macros */
#define LOG_INFO(fmt, ...)  \
    do { log_message(LOG_LEVEL_INFO, COLOR_GREEN, fmt, ##__VA_ARGS__); } while (0)

#define LOG_WARN(fmt, ...)  \
    do { log_message(LOG_LEVEL_WARN, COLOR_YELLOW, fmt, ##__VA_ARGS__); } while (0)

#define LOG_ERROR(fmt, ...) \
    do { log_message(LOG_LEVEL_ERROR, COLOR_RED, "[%s:%d] " fmt, \
                     __FILE__, __LINE__, ##__VA_ARGS__); } while (0)

#define LOG_SUCCESS(fmt, ...)  \
    do { log_message(LOG_LEVEL_SUCCESS, COLOR_GREEN, fmt, ##__VA_ARGS__); } while (0)

#ifdef DEBUG_MODE
#define LOG_DEBUG(fmt, ...) \
    do { log_message(LOG_LEVEL_DEBUG, COLOR_CYAN, "[%s:%d] " fmt, \
                     __FILE__, __LINE__, ##__VA_ARGS__); } while (0)
#else
#define LOG_DEBUG(fmt, ...) do {} while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
