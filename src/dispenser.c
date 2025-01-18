#include <stdio.h>
#include "../include/dispenser.h"

Product_dispenser g_dispenser[NUM_PRODUCTS];

void init_dispensers() {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        sprintf(g_dispenser[i].name, "Product %d", i + 1);
        g_dispenser[i].price = (float)(5 + rand() % 20);
        g_dispenser[i].quantity = 10;
        g_dispenser[i].capacity = MAX_CAPACITY;
    }
}

void show_dispenser() {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        printf("Name: %s\n", g_dispenser[i].name);
        printf("Price: %.2f\n", g_dispenser[i].price);
        printf("Quantity: %d\n", g_dispenser[i].quantity);
        printf("Capacity: %d\n", g_dispenser[i].capacity);
        printf("\n");
    }
}

void decrease_quantity(int product_id, int quantity) {
    int target_id = product_id;
    Product_dispenser *target = &g_dispenser[target_id];

    if (target->quantity >= quantity) {
        printf("%d\n", target->quantity);
        target->quantity -= quantity;
        printf("%d\n", target->quantity);
    } else {
        printf("Not enough products in the dispenser.\n");
    }
}