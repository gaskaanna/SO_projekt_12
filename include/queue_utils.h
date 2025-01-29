#ifndef QUEUE_UTILS_H
#define QUEUE_UTILS_H

#include <stdbool.h>
#include <sys/types.h>
#include "queue_types.h"

void init_queue(CashierQueue *queue);
bool enqueue(CashierQueue *queue, pid_t client_pid);
pid_t dequeue(CashierQueue *queue);
bool is_queue_empty(CashierQueue *queue);
bool is_queue_full(CashierQueue *queue);
int get_queue_position(CashierQueue *queue, pid_t client_pid);

#endif // QUEUE_UTILS_H