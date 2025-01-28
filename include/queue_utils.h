#ifndef SO_PROJEKT_QUEUE_UTILS_H
#define SO_PROJEKT_QUEUE_UTILS_H

#include <stdbool.h>
#include <sys/types.h>
#include "cashier.h"

void init_queue(CashierQueue *queue);
bool enqueue(CashierQueue *queue, pid_t client_pid);
pid_t dequeue(CashierQueue *queue);
bool is_queue_empty(CashierQueue *queue);
bool is_queue_full(CashierQueue *queue);
int get_queue_position(CashierQueue *queue, pid_t client_pid);

#endif