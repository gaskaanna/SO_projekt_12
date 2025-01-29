#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "../include/dispenser.h"
#include "../include/shared_memory.h"

void init_dispensers() {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        shared_memory->dispensers[i].id = i;
        sprintf(shared_memory->dispensers[i].name, "Product %d", i + 1);
        shared_memory->dispensers[i].price = (float)(5 + rand() % 20);
        shared_memory->dispensers[i].quantity = 0;
        shared_memory->dispensers[i].capacity = MAX_CAPACITY;
        shared_memory->dispensers[i].total_produced = 0;
        shared_memory->dispensers[i].total_sold = 0;
    }
}

void decrease_quantity(int product_id, int quantity) {
    Product_dispenser *target = &shared_memory->dispensers[product_id];

    if (target->quantity >= quantity) {
        target->quantity -= quantity;
        target->total_sold += quantity;
    }
}