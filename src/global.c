#include <stdio.h>
#include "../include/global.h"

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