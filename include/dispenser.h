#ifndef SO_PROJEKT_DISPENSER_H
#define SO_PROJEKT_DISPENSER_H

#include "Product.h"

#define MAX_CAPACITY 20

typedef struct {
    Product product;
    int quantity;
    int capacity;
} Product_dispenser;

extern Product_dispenser g_dispenser[NUM_PRODUCTS];

void init_dispenser();
void show_dispenser();



#endif //SO_PROJEKT_DISPENSER_H
