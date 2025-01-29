#ifndef SO_PROJEKT_CASHIER_H
#define SO_PROJEKT_CASHIER_H

#include <stdbool.h>
#include "queue_types.h"
#include "queue_utils.h"

typedef struct {
    int id;
    bool is_open;
} Cashier;

void init_cashier_queue(CashierQueue *queue);
void init_cashiers(void);
void cashier_process(int id);

#endif //SO_PROJEKT_CASHIER_H

