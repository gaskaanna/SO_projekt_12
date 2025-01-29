#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

#define MAX_LOG_LENGTH 256

typedef enum {
    PROCESS_MAIN,
    PROCESS_BAKER,
    PROCESS_CASHIER,
    PROCESS_CLIENT,
} ProcessType;

void init_logging(const char* base_filename);
void send_log(ProcessType sender, const char *format, ...);
void cleanup_logging(void);

#endif

