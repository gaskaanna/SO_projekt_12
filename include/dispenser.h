#ifndef SO_PROJEKT_DISPENSER_H
#define SO_PROJEKT_DISPENSER_H

#define NUM_PRODUCTS 12
#define MAX_CAPACITY 20

typedef struct {
    int id;
    char name[50];
    float price;
    int quantity;
    int capacity;
} Product_dispenser;

extern Product_dispenser g_dispenser[NUM_PRODUCTS];

void init_dispensers();
void show_dispenser();

void decrease_quantity(int product_id, int quantity);

#endif //SO_PROJEKT_DISPENSER_H
