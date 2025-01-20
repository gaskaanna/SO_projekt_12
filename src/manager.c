#include "stdio.h"
#include <pthread.h>
#include <stdbool.h>
#include "unistd.h"
#include "../include/manager.h"
#include "../include/cashier.h"

#define TIME_TO_CLOSE 60

void open_shop() {
    pthread_mutex_lock(&g_mutex);
    g_storeOpen = true;
    pthread_cond_broadcast(&g_condStore);
    pthread_mutex_unlock(&g_mutex);
}

void close_shop() {
    pthread_mutex_lock(&g_mutex);
    g_storeOpen = false;
    pthread_mutex_unlock(&g_mutex);
}

int calculate_cashiers_needed(int currentClients) {
    if (MAX_CLIENTS_PER_CASHIER <= 0) {
        return 1;
    };

    return ((currentClients + MAX_CLIENTS_PER_CASHIER - 1) / MAX_CLIENTS_PER_CASHIER);
}

void* manager_thread(void* arg) {
    int timeCounter = 0;

    while (1) {
        sleep(1);
        timeCounter++;
        printf("Manager work time: %d\n", timeCounter);

        if (timeCounter == 5) {
            open_shop();
        }

        if (timeCounter > TIME_TO_CLOSE) {
            close_shop();
            break;
        }

        pthread_mutex_lock(&g_mutex);

        int currentClients = g_currentClientsInStore;
        int neededCashiers = calculate_cashiers_needed(currentClients);

        printf("Current clients: %d\n", currentClients);

        for(int c = 0; c < NUM_CASHIERS; c++) {
            g_cashiers[c].is_open = c < neededCashiers;
        }

        pthread_cond_broadcast(&g_condCashier);
        pthread_mutex_unlock(&g_mutex);
    }

    pthread_exit(NULL);
}

