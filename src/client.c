#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/client.h"

#include "../include/cashier.h"
#include "../include/global.h"

#define MIN_CLIENT_JOIN_TIME 1
#define MAX_CLIENT_JOIN_TIME 2

#define MIN_CLIENTS_PER_BATCH 1
#define MAX_CLIENTS_PER_BATCH 4

#define MIN_NEED_PER_PRODUCT 0
#define MAX_NEED_PER_PRODUCT 12

#define SHOPPING_TIME 1

void get_products_from_shopping_list(Client *client) {
    for(int i = 0; i < NUM_PRODUCTS; i++) {
        int currentDispenserQuantity = g_dispenser[i].quantity;
        int needed = client->shoppingList[i].needed;

        printf("\n\n[CLIENT %d] Dispenser ID %d has quantity before take %d \n", client->id, i, g_dispenser[i].quantity);

        if (needed > 0 && currentDispenserQuantity > 0) {
            int quantityToTake = needed > currentDispenserQuantity ? currentDispenserQuantity : needed;

            pthread_mutex_lock(&g_mutex);
            decrease_quantity(i, quantityToTake);
            pthread_mutex_unlock(&g_mutex);

            client->shoppingList[i].needed -= quantityToTake;
            client->shoppingList[i].taken += quantityToTake;

            printf("[CLIENT %d] Took %d products of type %d\n", client->id, quantityToTake, i);
        } else {
            printf("[CLIENT %d] Doesn't toke products of type %d\n", client->id, i);
        }

        printf("[CLIENT %d] Dispenser ID %d has quantity %d \n\n", i, g_dispenser[i].quantity);
    }
}

void enqueueClient(CashierQueue* q, Client* client) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == MAX_CLIENTS_PER_CASHIER && g_storeOpen) {
        pthread_cond_wait(&q->notFull, &q->mutex);
    }
    pthread_mutex_unlock(&q->mutex);

    pthread_mutex_lock(&q->mutex);

    q->clients[q->rear] = client;
    q->rear = (q->rear + 1) % MAX_CLIENTS_PER_CASHIER;
    q->count++;

    if (!g_storeOpen) {
        return;
    }

    pthread_cond_signal(&q->notEmpty);
    pthread_mutex_unlock(&q->mutex);
}

void* client_thread(void* arg) {
    Client* client = (Client*)arg;

    printf("[CLIENT %d] Entered the store.\n", client->id);
    pthread_mutex_lock(&g_mutex);
    g_currentClientsInStore++;
    pthread_mutex_unlock(&g_mutex);

    sleep(SHOPPING_TIME);
    get_products_from_shopping_list(client);

    pthread_mutex_lock(&g_mutex);

    if (!g_storeOpen) {
        printf("[CLIENT %d] Store closed right after shopping – client leaves.\n", client->id);
        free(client);
        g_currentClientsInStore--;
        pthread_mutex_unlock(&g_mutex);
        pthread_exit(NULL);
    }

    int openedCashiers[NUM_CASHIERS];
    int openedCount = 0;
    for (int i = 0; i < NUM_CASHIERS; i++) {
        if (g_cashiers[i].is_open) {
            openedCashiers[openedCount++] = i;
        }
    }
    pthread_mutex_unlock(&g_mutex);

    if (openedCount == 0) {
        printf("[CLIENT %d] No open cashiers, client leaves.\n", client->id);
        free(client);
        pthread_mutex_lock(&g_mutex);
        g_currentClientsInStore--;
        pthread_mutex_unlock(&g_mutex);
        pthread_exit(NULL);
    }

    int chosenIdx = rand() % openedCount;
    int chosenCashierId = openedCashiers[chosenIdx];
    printf("[CLIENT %d] Joins queue of cashier %d\n", client->id, chosenCashierId);

    enqueueClient(&g_cashiers[chosenCashierId].queue, client);

    if (openedCount == 0) {
        printf("[CLIENT %d] No open cashiers, client leaves.\n", client->id);
        free(client);
        pthread_mutex_lock(&g_mutex);
        g_currentClientsInStore--;
        pthread_mutex_unlock(&g_mutex);
        pthread_exit(NULL);
    }

    pthread_mutex_lock(&g_mutex);
    pthread_cond_wait(&client->served, &g_mutex);
    pthread_mutex_unlock(&g_mutex);

    printf("[CLIENT %d] Leaves cashier %d and exits store.\n", client->id, chosenCashierId);

    free(client);

    pthread_mutex_lock(&g_mutex);
    g_currentClientsInStore--;
    pthread_mutex_unlock(&g_mutex);

    pthread_exit(NULL);
}

Client* init_and_get_client_info(int id) {
    Client* client = malloc(sizeof(Client));

    client->id = id;
    client->isShopping = false;

    pthread_cond_init(&client->served, NULL);

    for(int j = 0; j < NUM_PRODUCTS; j++) {
        int neededQuantity = (rand() % (MAX_NEED_PER_PRODUCT - MIN_NEED_PER_PRODUCT + 1)) + MIN_NEED_PER_PRODUCT;

        client->shoppingList[j].productId = j;
        client->shoppingList[j].needed = neededQuantity;
        client->shoppingList[j].taken = 0;
    }

    return client;
}

void init_clients() {
    pthread_mutex_lock(&g_mutex);
    while (!g_storeOpen) {
        pthread_cond_wait(&g_condStore, &g_mutex);
    }
    pthread_mutex_unlock(&g_mutex);

    int client_id = 0;

    while (g_storeOpen) {
        sleep(1);

        int interval_sec = (rand() % (MAX_CLIENT_JOIN_TIME - MIN_CLIENT_JOIN_TIME + 1)) + MIN_CLIENT_JOIN_TIME;
        sleep(interval_sec);

        if (!g_storeOpen) {
            break;
        }

        int batch_size = (rand() % (MAX_CLIENTS_PER_BATCH - MIN_CLIENTS_PER_BATCH + 1)) + MIN_CLIENTS_PER_BATCH;

        for (int i = 0; i < batch_size; i++) {
            Client* client = init_and_get_client_info(client_id);

            pthread_t clientT;
            pthread_create(&clientT, NULL, client_thread, (void*)client);

            pthread_detach(clientT);

            client_id++;
        }
    }

    printf("[INIT_CLIENTS] Store closed. Stoped creating clients.\n");
}