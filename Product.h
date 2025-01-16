#ifndef SO_PROJEKT_PRODUCT_H
#define SO_PROJEKT_PRODUCT_H

typedef struct {
    int id;
    int quantity;   // current number of items in the dispenser
    int capacity;   // maximum capacity (Ki)
    float price;    // product price
} Product;

void init_products();
void show_products();

#endif //SO_PROJEKT_PRODUCT_H


