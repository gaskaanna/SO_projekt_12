#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/cashier.h"
#include "../include/global.h"

Cashier g_cashiers[NUM_CASHIERS];

void* cashier_thread(void* arg) {
    int cashierId = *((int*)arg);
    free(arg);

    pthread_mutex_lock(&g_mutex);
    while (!g_storeOpen) {
        pthread_cond_wait(&g_condStore, &g_mutex);
    }
    pthread_mutex_unlock(&g_mutex);

    while(1) {

        printf("Kasa nr %d zaczela prace\n", cashierId + 1);

        if(g_cashiers[cashierId].is_open) {
            printf("Kasa nr %d otwarta\n", cashierId + 1);
            break;
        }

        printf("Kasa nr %d zamknieta\n", cashierId + 1);

        break;
    }

}

void init_cashiers() {
    pthread_t cashiers[NUM_CASHIERS];

    for (int i = 0; i < NUM_CASHIERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;

        bool isOpen = i == 0;

        g_cashiers[i].id = i;
        g_cashiers[i].is_open = isOpen;

        for (int j = 0; j < NUM_PRODUCTS; j++) {
            g_cashiers[i].product_sold_list[j].productId = j;
            g_cashiers[i].product_sold_list[j].quantity = 0;
        }

        pthread_create(&cashiers[i], NULL, cashier_thread, (void*)id);

        pthread_join(cashiers[i], NULL);
    }
}
