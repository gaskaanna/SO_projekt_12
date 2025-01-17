#include <stdio.h>
#include "../include/dispenser.h"
//#include "../include/product.h"

Product_dispenser g_dispenser[NUM_PRODUCTS];

void init_dispenser() {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        g_dispenser[i].product = g_products[i];
        g_dispenser[i].quantity = 0;
        g_dispenser[i].capacity = MAX_CAPACITY;
    }
}

void show_dispenser() {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        printf("Product ID: %d\n", g_dispenser[i].product.id);
        printf("Name: %s\n", g_dispenser[i].product.name);
        printf("Price: %.2f\n", g_dispenser[i].product.price);
        printf("Quantity: %d\n", g_dispenser[i].quantity);
        printf("Capacity: %d\n", g_dispenser[i].capacity);
        printf("\n");
    }
}
