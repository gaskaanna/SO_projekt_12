#include <stdbool.h>
#include "dispenser.h"
#include "manager.h"
#include "client.h"

#ifndef SO_PROJEKT_CASHIER_H
#define SO_PROJEKT_CASHIER_H

#define NUM_CASHIERS 3

#define MAX_CLIENTS_PER_CASHIER (MAX_CLIENTS_IN_STORE / NUM_CASHIERS)


typedef struct {
    Client* clients[MAX_CLIENTS_PER_CASHIER];
    int front;
    int rear;
    int count;

    pthread_mutex_t mutex;
    pthread_cond_t notEmpty;
    pthread_cond_t notFull;
} CashierQueue;

typedef struct {
    int productId;
    int quantity;
} Cashier_product_sold_list;

typedef struct {
    int id;
    bool is_open;
    Cashier_product_sold_list product_sold_list[NUM_PRODUCTS];
    CashierQueue queue;
} Cashier;

extern Cashier g_cashiers[NUM_CASHIERS];

void init_cashiers();

#endif //SO_PROJEKT_CASHIER_H
