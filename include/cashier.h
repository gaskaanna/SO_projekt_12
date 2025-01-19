#ifndef SO_PROJEKT_CASHIER_H
#define SO_PROJEKT_CASHIER_H
#include <stdbool.h>
#include <pthread.h>
#include "dispenser.h"
#include "global.h"

#define NUM_CASHIERS 3

typedef struct{
    int productId;
    int quantity;
} Cashier_product_sold_list;

typedef struct{
    Cashier_product_sold_list product_sold_list[NUM_PRODUCTS];
    int id;
    bool is_open;
} Cashier;

void init_cashiers();

#endif //SO_PROJEKT_CASHIER_H
