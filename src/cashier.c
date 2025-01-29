#define _POSIX_C_SOURCE 200809L

#include "../include/cashier.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include "../include/shared_memory.h"
#include "../include/logs.h"
#include "../include/queue_utils.h"

volatile sig_atomic_t cashier_running = 1;
volatile sig_atomic_t cashier_active = 0;
volatile sig_atomic_t last_signal = 0;

int current_cashier_id;

void sleep_for_microseconds(long microseconds) {
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    nanosleep(&ts, NULL);
}

void cashier_signal_handler(int signum) {
    last_signal = signum;

    if (signum == SIGTERM) cashier_running = 0;
    if (signum == SIGUSR1) cashier_active = 1;
    if (signum == SIGUSR2) cashier_active = 0;
}

void init_cashier_signals(void) {
    struct sigaction sa;
    sa.sa_handler = cashier_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("Error setting up SIGTERM handler");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error setting up SIGUSR1 handler");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("Error setting up SIGUSR2 handler");
        exit(EXIT_FAILURE);
    }
}

void read_cashier_signal(int id) {
    if (last_signal == 0) return;

    if (last_signal == SIGTERM) send_log(PROCESS_CASHIER, "Cashier %d shutting down...\n", id);
    if (last_signal == SIGUSR1) {
        shared_memory->cashier_active[id] = true;
        send_log(PROCESS_CASHIER, "Cashier %d opening register\n", id);
    }
    if (last_signal == SIGUSR2) {
        shared_memory->cashier_active[id] = false;
        send_log(PROCESS_CASHIER, "Cashier %d closing register\n", id);
    }

    last_signal = 0;
}

void log_cashier_status(int id) {
    send_log(PROCESS_CASHIER, "Cashier %d status: %s, queue size: %d\n", id,
            shared_memory->cashier_active[id] ? "active" : "inactive",
            shared_memory->cashier_queues[id].size);
}

void init_cashier_queue(CashierQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    for (int i = 0; i < MAX_CLIENTS_PER_CASHIER; i++) {
        queue->client_pids[i] = 0;
    }
}

void serve_next_client(int cashier_id) {
    CashierQueue *queue = &shared_memory->cashier_queues[cashier_id];

    if (!is_queue_empty(queue)) {
        pid_t client_pid = dequeue(queue);

        send_log(PROCESS_CASHIER, "Cashier %d starting to serve client %d\n",
                cashier_id, client_pid);

        for (int i = 0; i < NUM_PRODUCTS; i++) {
            if (shared_memory->dispensers[i].quantity > 0) {
                int amount_to_sell = 1 + rand() % 3;
                if (amount_to_sell > shared_memory->dispensers[i].quantity) {
                    amount_to_sell = shared_memory->dispensers[i].quantity;
                }
                shared_memory->dispensers[i].quantity -= amount_to_sell;
                shared_memory->dispensers[i].total_sold += amount_to_sell;

                send_log(PROCESS_CASHIER, "Cashier %d sold %d products from dispenser %d to client %d\n",
                        cashier_id, amount_to_sell, i, client_pid);
            }
        }

        sleep(2);
        kill(client_pid, SIGUSR1);

        send_log(PROCESS_CASHIER, "Cashier %d finished serving client %d\n",
                cashier_id, client_pid);
    }
}

void cashier_process(int id) {
    current_cashier_id = id;
    init_cashier_signals();

    send_log(PROCESS_CASHIER, "Cashier %d started process\n", id);

    if(id == 0) {
        shared_memory->cashier_active[id] = true;
        cashier_active = true;
    }

    while (cashier_running) {
        read_cashier_signal(id);
        log_cashier_status(id);

        if (cashier_active && !is_queue_empty(&shared_memory->cashier_queues[id])) {
            serve_next_client(id);
        }

        sleep(1);
    }

    while (is_queue_empty(&shared_memory->cashier_queues[id])) {
        serve_next_client(id);
        sleep(1);
    }

    send_log(PROCESS_CASHIER, "Cashier %d ended\n", id);
}

void init_cashiers() {
    for (size_t i = 0; i < NUMBER_OF_CASHIERS; i++) {
        pid_t cashier_pid = fork();

        if (cashier_pid < 0) {
            perror("Error creating cashier process");
            exit(EXIT_FAILURE);
        }

        if (cashier_pid == 0) {
            cashier_process(i);
            exit(EXIT_SUCCESS);
        } else {
            shared_memory->cashier_pids[i] = cashier_pid;
            printf("[MAIN] Created cashier %zu with PID: %d\n", i, cashier_pid);
        }
    }
}