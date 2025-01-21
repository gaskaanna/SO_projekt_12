#include "global.h"
#include "dispenser.h"

#ifndef SO_PROJEKT_CLIENT_H
#define SO_PROJEKT_CLIENT_H

#define NUM_CLIENTS 10

typedef struct {
    int productId;
    int taken;
    int needed;
} Client_shopping_list_item;

typedef struct {
    int id;
    bool isShopping;
    pthread_cond_t served;
    Client_shopping_list_item shoppingList[NUM_PRODUCTS];
} Client;

void init_clients();

#endif //SO_PROJEKT_CLIENT_H
