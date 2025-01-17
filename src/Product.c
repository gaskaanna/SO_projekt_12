#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/Product.h"

Product g_products[NUM_PRODUCTS];

void init_products(){
    srand((unsigned int)time(NULL));

    for(int i = 0; i < NUM_PRODUCTS; i++) {
        g_products[i].id = i;
        sprintf(g_products[i].name, "Product %d", i + 1);
        g_products[i].price = (float)(5 + rand() % 20); // cena w przedziale np. 5-24
    }
}

void show_products() {
    for(int i = 0; i < NUM_PRODUCTS; i++) {
        printf("Product ID: %d\n", g_products[i].id);
        printf("Name: %s\n", g_products[i].name);
        printf("Price: %.2f\n", g_products[i].price);
        printf("\n");
    }
}