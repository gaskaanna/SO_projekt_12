#include "../include/cashier.h"
#include "../include/queue_utils.h"

void init_queue(CashierQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    for (int i = 0; i < MAX_CLIENTS_PER_CASHIER; i++) {
        queue->client_pids[i] = 0;
    }
}

bool enqueue(CashierQueue *queue, pid_t client_pid) {
    if (is_queue_full(queue)) {
        return false;
    }

    queue->client_pids[queue->rear] = client_pid;
    queue->rear = (queue->rear + 1) % MAX_CLIENTS_PER_CASHIER;
    queue->size++;
    return true;
}

pid_t dequeue(CashierQueue *queue) {
    if (is_queue_empty(queue)) {
        return -1;
    }

    pid_t client_pid = queue->client_pids[queue->front];
    queue->client_pids[queue->front] = 0;
    queue->front = (queue->front + 1) % MAX_CLIENTS_PER_CASHIER;
    queue->size--;
    return client_pid;
}

bool is_queue_empty(CashierQueue *queue) {
    return queue->size == 0;
}

bool is_queue_full(CashierQueue *queue) {
    return queue->size >= MAX_CLIENTS_PER_CASHIER;
}

int get_queue_position(CashierQueue *queue, pid_t client_pid) {
    if (is_queue_empty(queue)) {
        return -1;
    }

    int position = 1;  // Pozycja zaczyna się od 1
    int current = queue->front;
    int count = 0;

    while (count < queue->size) {
        if (queue->client_pids[current] == client_pid) {
            return position;
        }
        current = (current + 1) % MAX_CLIENTS_PER_CASHIER;
        position++;
        count++;
    }

    return -1;  // Klient nie został znaleziony w kolejce
}