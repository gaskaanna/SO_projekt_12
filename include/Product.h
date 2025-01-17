#ifndef SO_PROJEKT_PRODUCT_H
#define SO_PROJEKT_PRODUCT_H

#define NUM_PRODUCTS 12

typedef  struct {
    int id;
    char name[50];
    float price;
} Product;

extern Product g_products[NUM_PRODUCTS];

void init_products();
void show_products();

#endif //SO_PROJEKT_PRODUCT_H


