#ifndef SO_PROJEKT_CASHIER_H
#define SO_PROJEKT_CASHIER_H

#include <stdbool.h>
#include <sys/types.h>

#define NUMBER_OF_CASHIERS 3
#define MAX_CLIENTS_IN_STORE 30
#define MAX_CLIENTS_PER_CASHIER (MAX_CLIENTS_IN_STORE / NUMBER_OF_CASHIERS)

typedef struct {
    pid_t client_pids[MAX_CLIENTS_PER_CASHIER];
    int front;
    int rear;
    int size;
} CashierQueue;

typedef struct {
    int id;
    bool is_open;
} Cashier;

void init_cashiers(void);

#endif //SO_PROJEKT_CASHIER_H

