#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/global.h"
#include "../include/manager.h"
#include "../include/cashier.h"

#define SERVE_TIME 2

Cashier g_cashiers[NUM_CASHIERS];

Client* dequeueClient(CashierQueue* q) {
    pthread_mutex_lock(&q->mutex);

    while (q->count == 0 && g_storeOpen) {
        pthread_cond_wait(&q->notEmpty, &q->mutex);
    }

    Client* client = q->clients[q->front];
    q->front = (q->front + 1) % MAX_CLIENTS_PER_CASHIER;
    q->count--;

    pthread_cond_signal(&q->notFull);
    pthread_mutex_unlock(&q->mutex);

    return client;
}

void* cashier_thread(void* arg) {
    int cashierId = *((int*)arg);
    free(arg);

    pthread_mutex_lock(&g_mutex);
    while (!g_storeOpen) {
        pthread_cond_wait(&g_condStore, &g_mutex);
    }
    pthread_mutex_unlock(&g_mutex);

    printf("[CASHIER %d] Thread started.\n", cashierId);

    while(1) {
        pthread_mutex_lock(&g_mutex);
        bool storeStillOpen = g_storeOpen;
        bool cashierOpen    = g_cashiers[cashierId].is_open;
        pthread_mutex_unlock(&g_mutex);

        pthread_mutex_lock(&g_mutex);
        while (!g_cashiers[cashierId].is_open && g_storeOpen) {
            pthread_cond_wait(&g_condCashier, &g_mutex);
        }
        storeStillOpen = g_storeOpen;
        cashierOpen    = g_cashiers[cashierId].is_open;
        pthread_mutex_unlock(&g_mutex);

        if (!storeStillOpen && !cashierOpen) {
            printf("[CASHIER %d] Store closed and cashier closed -> exit.\n", cashierId);
            break;
        }

        printf("[CASHIER %d] Store open: %d, Cashier open: %d\n", cashierId, storeStillOpen, cashierOpen);

        Client* client = dequeueClient(&g_cashiers[cashierId].queue);

        if (client != NULL && storeStillOpen) {
            printf("[CASHIER %d] Serving client %d...\n", cashierId, client->id);
            sleep(SERVE_TIME);

            pthread_mutex_lock(&g_mutex);
            pthread_cond_signal(&client->served);
            pthread_mutex_unlock(&g_mutex);

            printf("[CASHIER %d] Finished serving client %d.\n", cashierId, client->id);
        }
        else {
            pthread_mutex_lock(&g_mutex);
            storeStillOpen = g_storeOpen;
            pthread_mutex_unlock(&g_mutex);

            if (!storeStillOpen) {
                printf("[CASHIER %d] No clients and store closed -> exit.\n", cashierId);
                pthread_mutex_lock(&g_mutex);
                pthread_cond_signal(&client->served);
                pthread_mutex_unlock(&g_mutex);
                break;
            }
            sleep(1);
        }
    }

    pthread_exit(NULL);
}

void init_cashier_queue(CashierQueue* q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->notEmpty, NULL);
    pthread_cond_init(&q->notFull, NULL);
}

static void init_cashier_info(int id) {
    bool isOpen = id == 0;

    g_cashiers[id].id = id;
    g_cashiers[id].is_open = isOpen;

    for (int j = 0; j < NUM_PRODUCTS; j++) {
        g_cashiers[id].product_sold_list[j].productId = j;
        g_cashiers[id].product_sold_list[j].quantity = 0;
    }

    init_cashier_queue(&g_cashiers[id].queue);
}

void init_cashiers(pthread_t cashiers[NUM_CASHIERS]) {
    for (int i = 0; i < NUM_CASHIERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;

        init_cashier_info(i);


        pthread_create(&cashiers[i], NULL, cashier_thread, (void*)id);
    }

}

