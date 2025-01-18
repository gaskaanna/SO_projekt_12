#ifndef SO_PROJEKT_CLIENT_H
#define SO_PROJEKT_CLIENT_H

#include "Product.h"
#include <stdlib.h>

typedef struct {
    Product product;
    int quantity;
} ClientsProductItem;

typedef struct {
    int id;
    ClientsProductItem **shoppingList;
    size_t shoppingListSize;
} Client;

Client* Client_new(int id);

#endif //SO_PROJEKT_CLIENT_H
