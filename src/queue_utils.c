#include "../include/cashier.h"
#include "../include/queue_utils.h"

void init_queue(CashierQueue *queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
}

bool enqueue(CashierQueue *queue, pid_t client_pid) {
    if (is_queue_full(queue)) return false;

    queue->rear = (queue->rear + 1) % MAX_CLIENTS_PER_CASHIER;
    queue->client_pids[queue->rear] = client_pid;
    queue->size++;
    return true;
}

pid_t dequeue(CashierQueue *queue) {
    if (is_queue_empty(queue)) return -1;

    pid_t client_pid = queue->client_pids[queue->front];
    queue->front = (queue->front + 1) % MAX_CLIENTS_PER_CASHIER;
    queue->size--;
    return client_pid;
}

bool is_queue_empty(CashierQueue *queue) {
    return queue->size == 0;
}

bool is_queue_full(CashierQueue *queue) {
    return queue->size == MAX_CLIENTS_PER_CASHIER;
}

int get_queue_position(CashierQueue *queue, pid_t client_pid) {
    if (is_queue_empty(queue)) return -1;

    int position = 1;
    int index = queue->front;

    for (int i = 0; i < queue->size; i++) {
        if (queue->client_pids[index] == client_pid) {
            return position;
        }
        index = (index + 1) % MAX_CLIENTS_PER_CASHIER;
        position++;
    }

    return -1;
}