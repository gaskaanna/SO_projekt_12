#ifndef QUEUE_TYPES_H
#define QUEUE_TYPES_H

#include <sys/types.h>

#define MAX_CLIENTS_IN_STORE 30
#define NUMBER_OF_CASHIERS 3
#define MAX_CLIENTS_PER_CASHIER (MAX_CLIENTS_IN_STORE / NUMBER_OF_CASHIERS)

typedef struct {
    pid_t client_pids[MAX_CLIENTS_PER_CASHIER];
    int front;
    int rear;
    int size;
} CashierQueue;

#endif // QUEUE_TYPES_H