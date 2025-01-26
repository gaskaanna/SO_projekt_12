#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "../include/dispenser.h"
#include "../include/global.h"

Product_dispenser g_dispenser[NUM_PRODUCTS];

FILE* dispenserLogFile = NULL;


void init_dispenser_log() {
    char dispenserLogPath[300];
    snprintf(dispenserLogPath, sizeof(dispenserLogPath), "%s/dispenser.txt", g_logBasePath);

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

    dispenserLogFile = fopen( dispenserLogPath, "w");

    if (dispenserLogFile) {
        setvbuf(dispenserLogFile, NULL, _IOLBF, 0);
    } else {
        fprintf(stderr, "Could not open log file '%s': %s\n", dispenserLogPath, strerror(errno));
    }
}

void log_dispenser(const char *format, ...)
{

    if (dispenserLogFile) {
        va_list args;
        va_start(args, format);
        vfprintf(dispenserLogFile, format, args);
        va_end(args);

        fflush(dispenserLogFile);
    }
}

void close_dispenser_log(void)
{

    if (dispenserLogFile) {
        fclose(dispenserLogFile);
        dispenserLogFile = NULL;
    }

}

void init_dispensers() {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        sprintf(g_dispenser[i].name, "Product %d", i + 1);
        g_dispenser[i].price = (float)(5 + rand() % 20);
        g_dispenser[i].quantity = 0;
        g_dispenser[i].capacity = MAX_CAPACITY;
    }
}

void show_dispenser() {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        printf("[DISPENSER %d] Name: %s\n", i, g_dispenser[i].name);

        global_log_main("[time=%d]  Dispenser %d name: %s.\n",g_timeCounter,i, g_dispenser[i].name);
        log_dispenser("[time=%d]  Dispenser %d name: %s.\n",g_timeCounter,i, g_dispenser[i].name);

        printf("[DISPENSER %d] Price: %.2f\n", i, g_dispenser[i].price);

        global_log_main("[time=%d]  Dispenser %d price: %.2f\n",g_timeCounter, i, g_dispenser[i].price);
        log_dispenser("[time=%d]  Dispenser %d price: %.2f\n",g_timeCounter, i, g_dispenser[i].price);

        printf("[DISPENSER %d] Quantity: %d\n", i, g_dispenser[i].quantity);

        global_log_main("[time=%d]  Dispenser %d quantity: %d\n", g_timeCounter,i, g_dispenser[i].quantity);
        log_dispenser("[time=%d]  Dispenser %d quantity: %d\n",g_timeCounter, i, g_dispenser[i].quantity);

        printf("[DISPENSER %d] Capacity: %d\n", i, g_dispenser[i].capacity);

        global_log_main("[time=%d]  Dispenser %d capacity: %d\n",g_timeCounter, i, g_dispenser[i].capacity);
        log_dispenser("[time=%d]  Dispenser %d capacity: %d\n",g_timeCounter, i, g_dispenser[i].capacity);

        printf("\n");
    }
}

void decrease_quantity(int product_id, int quantity) {
    int target_id = product_id;
    Product_dispenser *target = &g_dispenser[target_id];

    if (target->quantity >= quantity) {
        target->quantity -= quantity;
    }
}
