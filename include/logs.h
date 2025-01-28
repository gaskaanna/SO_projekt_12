#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

typedef enum {
    PROCESS_BAKER,
    PROCESS_CASHIER,
    PROCESS_CLIENT,
} ProcessType;

#define MAX_LOG_LENGTH 256

struct log_message {
    ProcessType sender;
    char content[MAX_LOG_LENGTH];
};

void send_log(ProcessType sender, const char *format, ...);
void read_log(void);

#endif