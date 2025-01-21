#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include "../include/baker.h"
#include "../include/global.h"

#define MIN_BAKING_TIME 2
#define MAX_BAKING_TIME 5

void finding_not_full_dispensers(int *dispensers_to_possible_to_update, int *count) {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        if (g_dispenser[i].quantity < g_dispenser[i].capacity) {
            dispensers_to_possible_to_update[*count] = i;
            (*count)++;
        }
    }
}

void adding_to_dispenser(int *dispensers_to_update, int num_dispenser_to_update) {
    for (int i = 0; i < num_dispenser_to_update; i++) {
        int target_id = dispensers_to_update[i];
        Product_dispenser *target = &g_dispenser[target_id];

        int available_space = target->capacity - target->quantity;

        if (available_space > 0) {
            int quantity_to_add = rand() % available_space + 1; // Random quantity between 1 and available_space
            target->quantity += quantity_to_add;
            printf("[BAKER] Added %d products to dispenser %d. New quantity: %d\n", quantity_to_add, target_id, target->quantity);
        } else {
            printf("[BAKER] Dispenser %d is full. Cannot add more products.\n", target_id);
        }
    }
}

void bake() {
    printf("[BAKER] Baking bread...\n");

    int dispensers_to_possible_to_update[NUM_PRODUCTS];
    int count = 0;

    finding_not_full_dispensers(dispensers_to_possible_to_update, &count);
    printf("[BAKER] Count of not full dispensers: %d\n", count);

    if(count < 2) {
        printf("[BAKER] Not enough dispensers with available space.\n");
        sleep(2);
        return;
    }

    int num_dispenser_to_update = rand() % (count - 1) + 2; // At least 2 dispensers
    int dispensers_to_update[num_dispenser_to_update];
    int used_indices[count];
    memset(used_indices, 0, sizeof(used_indices));

    for (int i = 0; i < num_dispenser_to_update; i++) {
        int random_index;

        do {
            random_index = rand() % count;
        } while (used_indices[random_index] == 1);

        used_indices[random_index] = 1;
        dispensers_to_update[i] = dispensers_to_possible_to_update[random_index];
    }

    for (int i = 0; i < num_dispenser_to_update; i++) {
        printf("%d ", dispensers_to_update[i]);
    }

    printf("\n");
    adding_to_dispenser(dispensers_to_update, num_dispenser_to_update);
    printf("[BAKER] Bread is ready!\n");
}

void* baker_thread(void *arg) {
    printf("[BAKER] Baker thread started\n");

    wait_for_store_open();

    while(1) {
        sleep(2);
        pthread_mutex_lock(&g_mutex);

        if(!g_storeOpen) {
            printf("[BAKER] Store is closed. Baker ends work.\n");
            pthread_mutex_unlock(&g_mutex);
            pthread_exit(NULL);
        }

        int bakingTime = MIN_BAKING_TIME + rand() % (MAX_BAKING_TIME - MIN_BAKING_TIME);

        sleep(bakingTime);
        printf("[BAKER] Baking time: %d\n", bakingTime);

        bake();

        pthread_mutex_unlock(&g_mutex);
    }
}
