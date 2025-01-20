#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/client.h"
#include "../include/global.h"

#define MIN_CLIENT_JOIN_TIME 1
#define MAX_CLIENT_JOIN_TIME 3

#define MIN_CLIENTS_PER_BATCH 1
#define MAX_CLIENTS_PER_BATCH 4

void* client_thread(void* arg) {
    Client* client = (Client*)arg;

    printf("Client ID %d joined\n", client->id);
    g_currentClientsInStore++;

    while(g_storeOpen) {
        sleep(1);
    }

    free(client);

    pthread_exit(NULL);
};

Client* init_and_get_client_info(int id) {
    Client* client = malloc(sizeof(Client));

    client->id = id;
    client->isShopping = false;

    for(int j = 0; j < NUM_PRODUCTS; j++) {
        client->shoppingList[j].productId = j;
        client->shoppingList[j].needed = 5;
        client->shoppingList[j].taken = 0;
    }

    return client;
}

void init_clients() {
    srand(time(NULL));

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