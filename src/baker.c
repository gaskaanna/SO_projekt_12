#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "../include/shared_memory.h"
#include "../include/logs.h"
#include "../include/baker.h"
#include "../include/dispenser.h"

#define MIN_BAKING_TIME 1
#define MAX_BAKING_TIME 3

volatile sig_atomic_t baker_running = 1;

void baker_signal_handler(int signum) {
    printf("\n[BAKER] Received signal %d. Preparing for shutdown...\n", signum);
    baker_running = 0;
}

void finding_not_full_dispensers(int *dispensers_to_possible_to_update, int *count) {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        if (shared_memory->dispensers[i].quantity < shared_memory->dispensers[i].capacity) {
            dispensers_to_possible_to_update[*count] = i;
            (*count)++;
        }
    }
}

void adding_to_dispenser(int *dispensers_to_update, int num_dispenser_to_update) {
    for (int i = 0; i < num_dispenser_to_update; i++) {
        int target_id = dispensers_to_update[i];
        Product_dispenser *target = &shared_memory->dispensers[target_id];

        int available_space = target->capacity - target->quantity;

        if (available_space > 0) {
            int quantity_to_add = rand() % available_space + 1;

            send_log(PROCESS_BAKER, "Products in dispenser %d - quantity: %d\n",
                    target_id, target->quantity);

            target->quantity += quantity_to_add;
            target->total_produced += quantity_to_add;
            send_log(PROCESS_BAKER, "Added %d products to dispenser %d. New quantity: %d (Total produced: %d)\n",
                    quantity_to_add, target_id, target->quantity, target->total_produced);
        } else {
            send_log(PROCESS_BAKER, "Dispenser %d is full. Cannot add more products.\n",
                    target_id);
        }
    }
}

void bake() {
    send_log(PROCESS_BAKER, "Baking bread...\n");

    int dispensers_to_possible_to_update[NUM_PRODUCTS];
    int count = 0;

    finding_not_full_dispensers(dispensers_to_possible_to_update, &count);
    send_log(PROCESS_BAKER, "Count of not full dispensers: %d\n", count);

    if(count < 2) {
        send_log(PROCESS_BAKER, "Not enough dispensers with available space.\n");
        sleep(2);
        return;
    }

    int max_dispensers = (count < 4) ? count : 4;
    int num_dispenser_to_update = 2 + (rand() % (max_dispensers - 1));

    send_log(PROCESS_BAKER, "Will update %d dispensers\n", num_dispenser_to_update);

    int dispensers_to_update[num_dispenser_to_update];
    int used_indices[count];
    memset(used_indices, 0, sizeof(used_indices));

    for (int i = 0; i < num_dispenser_to_update; i++) {
        int random_index;
        int attempts = 0;
        do {
            random_index = rand() % count;
            attempts++;
            if (attempts > 100) {
                send_log(PROCESS_BAKER, "Too many attempts to find unused dispenser, breaking\n");
                return;
            }
        } while (used_indices[random_index] == 1);

        used_indices[random_index] = 1;
        dispensers_to_update[i] = dispensers_to_possible_to_update[random_index];
        send_log(PROCESS_BAKER, "Selected dispenser %d for update\n",
                dispensers_to_possible_to_update[random_index]);
    }

    adding_to_dispenser(dispensers_to_update, num_dispenser_to_update);
    send_log(PROCESS_BAKER, "Bread is ready!\n");
}

void baker_process() {
    signal(SIGTERM, baker_signal_handler);
    srand(time(NULL) ^ getpid());
    send_log(PROCESS_BAKER, "Baker process started with PID: %d\n", shared_memory->baker_pid);

    while(baker_running) {
        send_log(PROCESS_BAKER, "Starting new baking cycle...\n");
        int bakingTime = MIN_BAKING_TIME + rand() % (MAX_BAKING_TIME - MIN_BAKING_TIME);
        send_log(PROCESS_BAKER, "Waiting for baking time: %d seconds\n", bakingTime);
        sleep(bakingTime);

        send_log(PROCESS_BAKER, "Baking time: %d\n", bakingTime);
        bake();

        send_log(PROCESS_BAKER, "Finished baking cycle...\n");
    }

    send_log(PROCESS_BAKER, "Baker process ended with PID: %d\n", shared_memory->baker_pid);
}

void init_baker() {
    pid_t baker_pid = fork();
    shared_memory->baker_pid = baker_pid;

    if(baker_pid < 0) {
        send_log(PROCESS_BAKER, "Error creating baker process\n");
        exit(EXIT_FAILURE);
    }
    if(baker_pid == 0) {
        baker_process();
        exit(EXIT_SUCCESS);
    }
}

