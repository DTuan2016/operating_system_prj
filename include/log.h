#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

// --- ANSI Color Codes ---
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

// --- Logging Macros ---
#define LOG_INFO(...)  log_message("INFO", COLOR_GREEN, __VA_ARGS__)
#define LOG_WARN(...)  log_message("WARN", COLOR_YELLOW, __VA_ARGS__)
#define LOG_ERROR(...) log_message("ERROR", COLOR_RED, __VA_ARGS__)
#define LOG_DEBUG(...) log_message("DEBUG", COLOR_CYAN, __VA_ARGS__)

void log_init(const char *filename);
void log_close();
void log_message(const char *level, const char *color, const char *fmt, ...);


#endif //LOG_H