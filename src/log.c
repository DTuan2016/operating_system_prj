#include "log.h"

static FILE *log_file = NULL;

void log_init(const char *filename){
    if(filename){
        log_file = fopen(filename, "a");
        if(!log_file){
            fprintf(stderr, COLOR_RED "[ERROR] Cannot open log file %s\n" COLOR_RESET, filename);
        }
        setvbuf(log_file, NULL, _IOLBF, 0); // flush theo dòng
    }
}

void log_close(){
    if(log_file){
        fclose(log_file);
        log_file = NULL;
    }
}

void log_message(const char *level, const char *color, const char *fmt, ...) {
    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);

    // --- Time stamp ---
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf),"%H:%M:%S", t);

    // --- Print to console ---
    fprintf(stdout, "%s[%s %s]%s", color, level, timebuf, COLOR_RESET);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    // --- Print to log file ---
    if(log_file){
        fprintf(log_file, "[%s %s]", level, timebuf);
        // va_list args_copy;
        // va_copy(args_copy, args);
        vfprintf(log_file, fmt, args_copy);
        // va_end(args_copy);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
    va_end(args_copy);
    va_end(args);
}