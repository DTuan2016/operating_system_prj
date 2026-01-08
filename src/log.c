#include "log.h"
#include <time.h>
#include <string.h>



static FILE *log_file = NULL;
static log_level_t g_log_level = LOG_LEVEL_WARN;
static const char* level_to_string(log_level_t level)
{
    switch (level) {
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_WARN:  return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        default: return "UNK";
    }
}



void log_init(const char *filename){

   

    if(filename){
        log_file = fopen(filename, "a");
        if(!log_file){
            fprintf(
                stderr, 
                COLOR_RED "[ERROR] Cannot open log file %s\n" COLOR_RESET, 
                filename);
            return;
        }
        setvbuf(log_file, NULL, _IOLBF, 0); // flush theo dòng
    }
}

void log_close(void){
    
    if(log_file){
        fclose(log_file);
        log_file = NULL;
    }
    
}

void log_set_level(log_level_t level)
{
    
    g_log_level = level;
    
}
log_level_t log_get_level(void)
{
    log_level_t level;
    level = g_log_level;
    
    return level;
}


void log_message(log_level_t level, const char *color, const char *fmt, ...) 
{
    
   
    if (level < g_log_level) return;

    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);

    // --- Time stamp ---
    time_t now = time(NULL);
    struct tm t;
    char timebuf[32];
    localtime_r(&now, &t);
    strftime(timebuf, sizeof(timebuf),"%H:%M:%S", &t);

    // --- Print to console ---
    fprintf(stdout, "%s[%s %s]%s ",
            color, level_to_string(level), timebuf, COLOR_RESET);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");

    // --- Print to log file ---
    if (log_file) {
        fprintf(log_file, "[%s %s] ",
                level_to_string(level), timebuf);
        vfprintf(log_file, fmt, args_copy);
        fprintf(log_file, "\n");
        fflush(log_file);
    }

    va_end(args_copy);
    va_end(args);
    
}