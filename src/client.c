#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../include/client.h"

#define MAX_PRODUCTS 12

ClientsProductItem* ClientsProductItem_new(int id, const char *name, float price, int quantity) {
    ClientsProductItem *item = (ClientsProductItem*)malloc(sizeof(ClientsProductItem));
    if (item == NULL) {
        fprintf(stderr, "Błąd alokacji pamięci dla ClientsProductItem.\n");
        exit(EXIT_FAILURE);
    }

    item->product.id = id;
    strncpy(item->product.name, name, sizeof(item->product.name) - 1);
    item->product.name[sizeof(item->product.name) - 1] = '\0';
    item->product.price = price;

    item->quantity = quantity;

    return item;
}

Client* Client_new(int id) {
    srand(time(NULL));
    int shoppingListSize = 0;
    Client *client = (Client*)malloc(sizeof(Client));

    if (client == NULL) {
        fprintf(stderr, "Błąd alokacji pamięci dla Client.\n");
        exit(EXIT_FAILURE);
    }

    client->id = id;
    client->shoppingList = (ClientsProductItem**)malloc(sizeof(ClientsProductItem*));

    if (client->shoppingList == NULL) {
        fprintf(stderr, "Błąd alokacji pamięci dla shoppingList.\n");
        free(client);
        exit(EXIT_FAILURE);
    }

    int num_products = rand() % MAX_PRODUCTS + 1;


    client->shoppingListSize = shoppingListSize;

    return client;
}

