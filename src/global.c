#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include "../include/global.h"
#include <direct.h>

int g_timeCounter = 0;

char g_logBasePath[256] = "";

FILE* g_mainLogFile = NULL;
pthread_mutex_t g_mainLogMutex = PTHREAD_MUTEX_INITIALIZER;

int g_currentClientsInStore = 0;
int g_storeOpen = 0;

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_condStore  = PTHREAD_COND_INITIALIZER;
pthread_cond_t g_condCashier  = PTHREAD_COND_INITIALIZER;

void wait_for_store_open() {
    pthread_mutex_lock(&g_mutex);

    while (!g_storeOpen) {
        pthread_cond_wait(&g_condStore, &g_mutex);
    }

    pthread_mutex_unlock(&g_mutex);
}


void global_init_main_log() {
    char mainLogPath[300];
    snprintf(mainLogPath, sizeof(mainLogPath), "%s/main.txt", g_logBasePath);

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

    g_mainLogFile = fopen( mainLogPath, "w");

    pthread_mutex_lock(&g_mainLogMutex);

    if (g_mainLogFile) {
        setvbuf(g_mainLogFile, NULL, _IOLBF, 0);
    } else {
        fprintf(stderr, "Could not open log file '%s': %s\n", mainLogPath, strerror(errno));
    }

    pthread_mutex_unlock(&g_mainLogMutex);
}


void global_close_main_log(void)
{
    pthread_mutex_lock(&g_mainLogMutex);

    if (g_mainLogFile) {
        fclose(g_mainLogFile);
        g_mainLogFile = NULL;
    }

    pthread_mutex_unlock(&g_mainLogMutex);
}

void global_log_main(const char *format, ...)
{

    pthread_mutex_lock(&g_mainLogMutex);

    if (g_mainLogFile) {
        va_list args;
        va_start(args, format);
        vfprintf(g_mainLogFile, format, args);
        va_end(args);

        fflush(g_mainLogFile);
    }

    pthread_mutex_unlock(&g_mainLogMutex);
}