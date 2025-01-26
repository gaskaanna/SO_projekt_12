#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <stdarg.h>
#include "../include/baker.h"
#include "../include/global.h"

FILE* bakerLogFile = NULL;

#define MIN_BAKING_TIME 2
#define MAX_BAKING_TIME 5

void init_baker_log() {
    char bakerLogPath[300];
    snprintf(bakerLogPath, sizeof(bakerLogPath), "%s/baker.txt", g_logBasePath);

    if (mkdir("../logs") == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Could not create directory 'logs': %s\n", strerror(errno));
        }
    }
    if (mkdir(g_logBasePath) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Could not create directory '%s': %s\n", g_logBasePath, strerror(errno));
        }
    }

    bakerLogFile = fopen( bakerLogPath, "w");

    if (bakerLogFile) {
        setvbuf(bakerLogFile, NULL, _IOLBF, 0);
    } else {
        fprintf(stderr, "Could not open log file '%s': %s\n", bakerLogPath, strerror(errno));
    }
}
void log_baker(const char *format, ...)
{

    if (bakerLogFile) {
        va_list args;
        va_start(args, format);
        vfprintf(bakerLogFile, format, args);
        va_end(args);

        fflush(bakerLogFile);
    }
}

void close_baker_log(void)
{
    if (bakerLogFile) {
        fclose(bakerLogFile);
        bakerLogFile = NULL;
    }
}

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

            log_baker("[time=%d]  Products in dispenser %d - quantity: %d\n", g_timeCounter, target_id, target->quantity);

            target->quantity += quantity_to_add;
            printf("[BAKER] Added %d products to dispenser %d. New quantity: %d\n", quantity_to_add, target_id, target->quantity);

            global_log_main("[time=%d]  Added %d products to dispenser %d. New quantity: %d\n",g_timeCounter ,quantity_to_add, target_id, target->quantity);
            log_baker("[time=%d]  Added %d products to dispenser %d. New quantity: %d\n",g_timeCounter ,quantity_to_add, target_id, target->quantity);
        } else {
            printf("[BAKER] Dispenser %d is full. Cannot add more products.\n", target_id);

            global_log_main("[time=%d]  Dispenser %d is full. Cannot add more products.\n",g_timeCounter ,target_id);
            log_baker("[time=%d]  Dispenser %d is full. Cannot add more products.\n",g_timeCounter ,target_id);
        }
    }
}

void bake() {
    printf("[BAKER] Baking bread...\n");

    global_log_main("[time=%d]  Baking bread...\n", g_timeCounter);
    log_baker("[time=%d]  Baking bread...\n", g_timeCounter);

    int dispensers_to_possible_to_update[NUM_PRODUCTS];
    int count = 0;

    finding_not_full_dispensers(dispensers_to_possible_to_update, &count);
    printf("[BAKER] Count of not full dispensers: %d\n", count);

    global_log_main("[time=%d]  Count of not full dispensers: %d\n",g_timeCounter, count);
    log_baker("[time=%d]  Count of not full dispensers: %d\n",g_timeCounter, count);

    if(count < 2) {
        printf("[BAKER] Not enough dispensers with available space.\n");
        global_log_main("[time=%d]  Not enough dispensers with available space.\n",g_timeCounter);
        log_baker("[time=%d]  Not enough dispensers with available space.\n",g_timeCounter);
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
    global_log_main("[time=%d]  Bread is ready!.\n",g_timeCounter);
    log_baker("[time=%d]  Bread is ready!\n",g_timeCounter);
}

void* baker_thread(void *arg) {
    init_baker_log();
    printf("[BAKER] Baker thread started\n");

    global_log_main("[time=%d] Baker thread started.\n", g_timeCounter);
    log_baker("[time=%d] Baker thread started.\n", g_timeCounter);

    wait_for_store_open();

    while(1)
    {
        //pthread_mutex_lock(&g_mutex);
    //int currentTime = g_timeCounter;
    //pthread_mutex_unlock(&g_mutex);


        sleep(2);
        pthread_mutex_lock(&g_mutex);
        if(!g_storeOpen) {
            printf("[BAKER] Store is closed. Baker ends work.\n");
            pthread_mutex_unlock(&g_mutex);

            global_log_main("[time=%d]  Store is closed. Baker ends work.\n", g_timeCounter);
            log_baker("[time=%d]  Store is closed. Baker ends work.\n", g_timeCounter);

            pthread_exit(NULL);


        }

        int bakingTime = MIN_BAKING_TIME + rand() % (MAX_BAKING_TIME - MIN_BAKING_TIME);

        sleep(bakingTime);
        printf("[BAKER] Baking time: %d\n", bakingTime);

        global_log_main("[time=%d]  Baking time: %d\n", g_timeCounter, bakingTime);
        log_baker("[time=%d]  Baking time: %d\n", g_timeCounter, bakingTime);

        bake();



        pthread_mutex_unlock(&g_mutex);
    }
}




