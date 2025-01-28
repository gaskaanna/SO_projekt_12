#ifndef SO_PROJEKT_DISPENSER_H
#define SO_PROJEKT_DISPENSER_H

#define NUM_PRODUCTS 12
#define MAX_CAPACITY 30

typedef struct {
    int id;
    char name[50];
    float price;
    int quantity;
    int capacity;
} Product_dispenser;

void init_dispensers(void);
void decrease_quantity(int product_id, int quantity);

#endif //SO_PROJEKT_DISPENSER_H