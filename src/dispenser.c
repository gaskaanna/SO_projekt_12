#include "stdio.h"
#include "stdlib.h"
#include "../include/dispenser.h"

Product_dispenser g_dispenser[NUM_PRODUCTS];

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
        printf("[DISPENSER %d] Price: %.2f\n", i, g_dispenser[i].price);
        printf("[DISPENSER %d] Quantity: %d\n", i, g_dispenser[i].quantity);
        printf("[DISPENSER %d] Capacity: %d\n", i, g_dispenser[i].capacity);
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