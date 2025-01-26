#include "stdio.h"
#include <pthread.h>
#include <stdbool.h>
#include "unistd.h"
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <direct.h>
#include "../include/global.h"
#include "../include/manager.h"
#include "../include/cashier.h"

#define TIME_TO_CLOSE 15


FILE* managerLogFile = NULL;

void init_manager_log() {
    char managerLogPath[300];
    snprintf(managerLogPath, sizeof(managerLogPath), "%s/manager.txt", g_logBasePath);

    if (mkdir("../logs") == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Could not create directory 'logs': %s\n", strerror(errno));
        }
    }
    if (mkdir(g_logBasePath) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Could not create directory '%s': %s\n", g_logBasePath, strerror(errno));
        }
    }

    managerLogFile = fopen( managerLogPath, "w");

    if (managerLogFile) {
        setvbuf(managerLogFile, NULL, _IOLBF, 0);
    } else {
        fprintf(stderr, "Could not open log file '%s': %s\n", managerLogPath, strerror(errno));
    }
}

void log_manager(const char *format, ...)
{

    if (managerLogFile) {
        va_list args;
        va_start(args, format);
        vfprintf(managerLogFile, format, args);
        va_end(args);

        fflush(managerLogFile);
    }
}

void close_manager_log(void)
{
    if (managerLogFile) {
        fclose(managerLogFile);
        managerLogFile = NULL;
    }
}

void open_shop() {
    pthread_mutex_lock(&g_mutex);
    g_storeOpen = true;
    printf("[MANAGER] Store is now OPEN!\n");
    pthread_mutex_unlock(&g_mutex);
}

void close_shop() {
    pthread_mutex_lock(&g_mutex);
    g_storeOpen = false;
    printf("[MANAGER] Store is now CLOSED!\n");
    pthread_mutex_unlock(&g_mutex);
}

void open_cashier(int cashierId) {
    pthread_mutex_lock(&g_mutex);
    if (!g_cashiers[cashierId].is_open) {
        g_cashiers[cashierId].is_open = true;
        printf("[MANAGER] Opened cashier %d\n", cashierId);
        pthread_cond_broadcast(&g_condCashier);
    }
    pthread_mutex_unlock(&g_mutex);
}

void close_cashier(int cashierId) {
    pthread_mutex_lock(&g_mutex);
    if (g_cashiers[cashierId].is_open) {
        g_cashiers[cashierId].is_open = false;
        printf("[MANAGER] Closed cashier %d\n", cashierId);
        pthread_cond_broadcast(&g_condCashier);
    }
    pthread_mutex_unlock(&g_mutex);
}

int calculate_cashiers_needed(int currentClients) {
    if (MAX_CLIENTS_PER_CASHIER <= 0) {
        return 1;
    };

    return ((currentClients + MAX_CLIENTS_PER_CASHIER - 1) / MAX_CLIENTS_PER_CASHIER);
}

void* manager_thread(void* arg) {
    init_manager_log();

    while (1) {
        sleep(1);
        pthread_mutex_lock(&g_mutex);
        g_timeCounter++;
        int currentTime = g_timeCounter;
        pthread_mutex_unlock(&g_mutex);

        printf("[MANAGER] Manager work time: %d\n", currentTime);

        global_log_main("[time=%d] Manager also logs to main.\n", currentTime);
        log_manager("[time=%d] Manager also logs to main.\n", currentTime);

        if (currentTime == 5) {
            open_shop();

            global_log_main("[time=%d] Manager opened shop.\n", currentTime);
            log_manager("[time=%d] Manager opened shop.\n", currentTime);
        }

        if (currentTime > TIME_TO_CLOSE) {
            close_shop();
            global_log_main("[time=%d] Manager closed shop.\n", currentTime);
            log_manager("[time=%d] Manager closed shop.\n", currentTime);
            break;
        }

        pthread_mutex_lock(&g_mutex);

        int currentClients = g_currentClientsInStore;
        int neededCashiers = calculate_cashiers_needed(currentClients);

        pthread_mutex_unlock(&g_mutex);

        printf("[MANAGER] Current clients: %d\n", currentClients);

        global_log_main("[time=%d] Manager current clients: %d.\n", currentTime, currentClients);
        log_manager("[time=%d] Manager current clients: %d.\n", currentTime, currentClients);

        if (g_storeOpen) {
            for(int c = 0; c < NUM_CASHIERS; c++) {
                if (c < neededCashiers) {
                    open_cashier(c);

                    global_log_main("[time=%d] Manager open cashier.\n", currentTime);
                    log_manager("[time=%d] Manager open cashier.\n", currentTime);

                } else {
                    close_cashier(c);

                    global_log_main("[time=%d] Manager closed cashier.\n", currentTime);
                    log_manager("[time=%d] Manager closed cashier.\n", currentTime);
                }
            }
        }
        pthread_mutex_lock(&g_mutex);
        pthread_cond_broadcast(&g_condStore);
        pthread_cond_broadcast(&g_condCashier);
        pthread_mutex_unlock(&g_mutex);
    }

    close_manager_log();
    pthread_exit(NULL);
}