#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/cashier.h"
#include "../include/global.h"

Cashier g_cashiers[NUM_CASHIERS];

void* cashier_thread(void* arg) {
    int cashierId = *((int*)arg);
    free(arg);

    wait_for_store_open();

    while(1) {
        sleep(1);
        printf("Kasa nr %d zaczela prace\n", cashierId + 1);

        if(g_cashiers[cashierId].is_open) {
            printf("Kasa nr %d otwarta\n", cashierId + 1);
            break;
        }

        printf("Kasa nr %d zamknieta\n", cashierId + 1);

        break;
    }

    pthread_exit(NULL);
}

void init_cashier_info(int id) {
    bool isOpen = id == 0;

    g_cashiers[id].id = id;
    g_cashiers[id].is_open = isOpen;

    for (int j = 0; j < NUM_PRODUCTS; j++) {
        g_cashiers[id].product_sold_list[j].productId = j;
        g_cashiers[id].product_sold_list[j].quantity = 0;
    }

}

void init_cashiers() {
    pthread_t cashiers[NUM_CASHIERS];

    for (int i = 0; i < NUM_CASHIERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;

        init_cashier_info(i);

        pthread_create(&cashiers[i], NULL, cashier_thread, (void*)id);

        pthread_join(cashiers[i], NULL);
    }
}


