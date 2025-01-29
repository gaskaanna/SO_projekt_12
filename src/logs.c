#define _DEFAULT_SOURCE
#include "../include/logs.h"
#include <string.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

#define FIFO_PATH "/tmp/bakery_logs_fifo"
#define MAX_PATH 256
#define MAX_MESSAGE_SIZE 512

static FILE *main_log = NULL;
static FILE *pid_log = NULL;
static sem_t sem_main;
static sem_t sem_pid;
static int fifo_fd = -1;
static pthread_t logging_thread;
static volatile int stop_logging = 0;

static void write_to_logs(const char* message) {
    printf("%s\n", message);

    sem_wait(&sem_main);
    if (main_log) {
        fprintf(main_log, "%s\n", message);
        fflush(main_log);
    }
    sem_post(&sem_main);

    sem_wait(&sem_pid);
    if (pid_log) {
        fprintf(pid_log, "%s\n", message);
        fflush(pid_log);
    }
    sem_post(&sem_pid);
}

static void* process_logs(void* arg __attribute__((unused))) {
    char buffer[MAX_MESSAGE_SIZE];
    ssize_t bytes_read;

    while (!stop_logging) {
        bytes_read = read(fifo_fd, buffer, MAX_MESSAGE_SIZE-1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            write_to_logs(buffer);
        } else if (bytes_read == 0) {
            close(fifo_fd);
            fifo_fd = open(FIFO_PATH, O_RDONLY | O_NONBLOCK);
        } else if (errno != EAGAIN) {
            perror("read");
        }
        usleep(100000);
    }
    return NULL;
}

void init_logging(const char* base_filename) {
    char main_filename[MAX_PATH];
    char pid_filename[MAX_PATH];
    char pid_dir[MAX_PATH];

    mkdir("logs", 0777);

    snprintf(main_filename, MAX_PATH, "%s.log", base_filename);
    main_log = fopen(main_filename, "a");
    if (!main_log) {
        perror("Failed to open main log file");
        exit(1);
    }

    snprintf(pid_dir, MAX_PATH, "%s", base_filename);
    mkdir(pid_dir, 0777);

    snprintf(pid_filename, MAX_PATH, "%s/%d.log", base_filename, getpid());
    pid_log = fopen(pid_filename, "a");
    if (!pid_log) {
        perror("Failed to open PID log file");
        exit(1);
    }

    sem_init(&sem_main, 0, 1);
    sem_init(&sem_pid, 0, 1);

    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo failed");
        exit(1);
    }

    fifo_fd = open(FIFO_PATH, O_RDWR | O_NONBLOCK);
    if (fifo_fd == -1) {
        perror("Failed to open FIFO");
        exit(1);
    }

    stop_logging = 0;
    if (pthread_create(&logging_thread, NULL, process_logs, NULL) != 0) {
        perror("Failed to create logging thread");
        exit(1);
    }
}

void send_log(ProcessType sender, const char* format, ...) {
    char final_message[MAX_MESSAGE_SIZE];
    char timestamp[32];
    time_t now;
    struct tm *tm_info;
    va_list args;

    time(&now);
    tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    va_start(args, format);
    char message[MAX_MESSAGE_SIZE - 64];
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    snprintf(final_message, sizeof(final_message), "[%s][%s] %s",
             timestamp,
             sender == PROCESS_MAIN ? "MAIN" :
             sender == PROCESS_BAKER ? "BAKER" :
             sender == PROCESS_CASHIER ? "CASHIER" :
             sender == PROCESS_CLIENT ? "CLIENT" : "UNKNOWN",
             message);

    write_to_logs(final_message);
}

void cleanup_logging(void) {
    stop_logging = 1;
    pthread_join(logging_thread, NULL);

    if (main_log) {
        fclose(main_log);
        main_log = NULL;
    }
    if (pid_log) {
        fclose(pid_log);
        pid_log = NULL;
    }

    if (fifo_fd != -1) {
        close(fifo_fd);
        fifo_fd = -1;
        unlink(FIFO_PATH);
    }

    sem_destroy(&sem_main);
    sem_destroy(&sem_pid);
}