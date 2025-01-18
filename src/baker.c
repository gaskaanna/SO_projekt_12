#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "time.h"
#include <stdbool.h>
#include <sys/wait.h>
#include "../include/baker.h"
#include "../include/dispenser.h"
#include "../include/Product.h"

int baker_exists = 0;

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
            printf("Added %d products to dispenser %d. New quantity: %d\n", quantity_to_add, target_id,
                   target->quantity);
        } else {
            printf("Dispenser %d is full. Cannot add more products.\n", target_id);
        }
    }
}

void bake() {
    printf("Baking bread...\n");

    srand(time(NULL));

    int dispensers_to_possible_to_update[NUM_PRODUCTS];
    int count = 0;

    finding_not_full_dispensers(dispensers_to_possible_to_update, &count);

    printf("Count: %d\n", count);

    if(count < 2) {
        printf("Not enough dispensers with available space.\n");
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


    printf("Number of dispensers to update: %d\n", num_dispenser_to_update);

    adding_to_dispenser(dispensers_to_update, num_dispenser_to_update);

//    sleep(2); // do usunięcia
   // sleep(random(time());
    printf("Bread is ready!\n");
}

void create_baker() {
    if (baker_exists) {
        printf("Baker already exists.\n");
        return;
    }
    pid_t baker_pid = fork();

    if(baker_pid < 0) {
        perror("fork() error");
        return;
    }

    if(baker_pid == 0) {
        baker_exists = 1;

        for (int i = 0; i < 1; i++) {
            bake();
            sleep(2);
        }
        printf("[PID: %d] Piekarz kończy pracę.\n", getpid());
        _exit(EXIT_SUCCESS);
    } else {
        baker_exists = 1;     // ustawiamy informację, że mamy już piekarza
        g_baker.id = baker_pid;
        printf("[PID: %d] Stworzono piekarza o PID = %d\n", getpid(), baker_pid);
    }
}
