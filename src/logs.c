
#include "../include/shared_memory.h"
#include "../include/logs.h"
#include <string.h>
#include <sys/select.h>

void send_log(ProcessType sender, const char *format, ...) {
    struct log_message msg;
    va_list args;

    msg.sender = sender;

    va_start(args, format);
    vsnprintf(msg.content, MAX_LOG_LENGTH, format, args);
    va_end(args);

    write(shared_memory->log_pipe[1], &msg, sizeof(struct log_message));
}

void read_log(void) {
    struct log_message msg;
    ssize_t bytes_read;

    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(shared_memory->log_pipe[0], &readfds);

    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    int ready = select(shared_memory->log_pipe[0] + 1, &readfds, NULL, NULL, &tv);

    if (ready == -1) {
        perror("select");
        return;
    }

    if (ready == 0) {
        return;
    }
    bytes_read = read(shared_memory->log_pipe[0], &msg, sizeof(struct log_message));
    if (bytes_read > 0) {
        const char *sender_str;
        switch (msg.sender) {
            case PROCESS_BAKER:
                sender_str = "BAKER";
                break;
            case PROCESS_CASHIER:
                sender_str = "CASHIER";
                break;
            case PROCESS_CLIENT:
                sender_str = "CLIENT";
                break;
            default:
                sender_str = "UNKNOWN";
        }
        printf("[%s] %s", sender_str, msg.content);
    } else if (bytes_read == 0) {
        printf("Pipe closed\n");
    } else {
        perror("read");
    }
}