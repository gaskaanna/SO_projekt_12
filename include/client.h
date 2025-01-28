#ifndef SO_PROJEKT_CLIENT_H
#define SO_PROJEKT_CLIENT_H

#include <stdbool.h>

#include "../include/dispenser.h"  // dla NUM_PRODUCTS

#define SHOPPING_TIME 1

typedef struct {
  int productId;
  int taken;
  int needed;
} Client_shopping_list_item;

typedef struct {
  int id;
  bool isShopping;
  Client_shopping_list_item shoppingList[NUM_PRODUCTS];
} Client;

void init_new_clients(int new_clients);

#endif //SO_PROJEKT_CLIENT_H