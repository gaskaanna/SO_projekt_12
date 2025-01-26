#include "stdio.h"
#include <pthread.h>
#include <stdbool.h>
#include "unistd.h"
#include "../include/manager.h"
#include "../include/cashier.h"

#define TIME_TO_CLOSE 15

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
    while (1) {
        sleep(1);
        pthread_mutex_lock(&g_mutex);
        g_timeCounter++;
        int currentTime = g_timeCounter;
        pthread_mutex_unlock(&g_mutex);

        printf("[MANAGER] Manager work time: %d\n", currentTime);

        global_log_main("[time=%d] Manager also logs to main.\n", currentTime);


        if (currentTime == 5) {
            open_shop();
        }

        if (currentTime > TIME_TO_CLOSE) {
            close_shop();
            break;
        }

        pthread_mutex_lock(&g_mutex);

        int currentClients = g_currentClientsInStore;
        int neededCashiers = calculate_cashiers_needed(currentClients);

        pthread_mutex_unlock(&g_mutex);

        printf("[MANAGER] Current clients: %d\n", currentClients);

        if (g_storeOpen) {
            for(int c = 0; c < NUM_CASHIERS; c++) {
                if (c < neededCashiers) {
                    open_cashier(c);

                } else {
                    close_cashier(c);
                }
            }
        }
        pthread_mutex_lock(&g_mutex);
        pthread_cond_broadcast(&g_condStore);
        pthread_cond_broadcast(&g_condCashier);
        pthread_mutex_unlock(&g_mutex);
    }

    pthread_exit(NULL);
}

