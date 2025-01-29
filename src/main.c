#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#include "../include/shared_memory.h"
#include "../include/main.h"
#include "../include/cashier.h"
#include "../include/logs.h"

#define MIN_INIT_CLIENTS 1
#define MAX_INIT_CLIENTS 3

pid_t my_pid;
int g_currentTime = 0;
int created_clients = 0;

int calculate_cashiers_needed(int currentClients) {
    int needed_cashiers = 1;

    for (int i = 1; i < NUMBER_OF_CASHIERS; i++) {
        if (currentClients > i * MAX_CLIENTS_PER_CASHIER) {
            needed_cashiers++;
        }
    }

    return needed_cashiers;
}

int calculate_new_clients() {
    int new_clients = rand() % (MAX_INIT_CLIENTS - MIN_INIT_CLIENTS + 1) + MIN_INIT_CLIENTS;
    return new_clients;
}

void activate_cashier(int cashier_id) {
    if (!shared_memory->cashier_active[cashier_id]) {
        if (kill(shared_memory->cashier_pids[cashier_id], SIGUSR1) == -1) {
            perror("Error sending SIGUSR1");
        }
    }
}

void deactivate_cashier(int cashier_id) {
    if (shared_memory->cashier_active[cashier_id]) {
        if (kill(shared_memory->cashier_pids[cashier_id], SIGUSR2) == -1) {
            perror("Error sending SIGUSR2");
        }
    }
}

void stop_simulation() {
    kill(shared_memory->baker_pid, SIGTERM);

    for(int i = 0; i < NUMBER_OF_CASHIERS; i++) {
        kill(shared_memory->cashier_pids[i], SIGTERM);
    }

    int status;
    waitpid(shared_memory->baker_pid, &status, 0);

    for(int i = 0; i < NUMBER_OF_CASHIERS; i++) {
        waitpid(shared_memory->cashier_pids[i], &status, 0);
    }
}

void start_evacuation(void) {
    send_log(PROCESS_MAIN, "EMERGENCY: Starting evacuation procedure!\n");

    if (shared_memory->baker_pid > 0) {
        send_log(PROCESS_MAIN, "Stopping baker process...\n");
        kill(shared_memory->baker_pid, SIGTERM);
        waitpid(shared_memory->baker_pid, NULL, 0);
    }

    for (int i = 0; i < NUMBER_OF_CASHIERS; i++) {
        if (shared_memory->cashier_pids[i] > 0) {
            send_log(PROCESS_MAIN, "Stopping cashier %d...\n", i);
            kill(shared_memory->cashier_pids[i], SIGTERM);
            waitpid(shared_memory->cashier_pids[i], NULL, 0);
        }
    }

    for (int i = 0; i < NUMBER_OF_CASHIERS; i++) {
        CashierQueue *queue = &shared_memory->cashier_queues[i];
        for (int j = 0; j < MAX_CLIENTS_PER_CASHIER; j++) {
            if (queue->client_pids[j] > 0) {
                send_log(PROCESS_MAIN, "Stopping client %d from queue %d...\n",
                        queue->client_pids[j], i);
                kill(queue->client_pids[j], SIGTERM);
                waitpid(queue->client_pids[j], NULL, 0);
            }
        }
    }

    send_log(PROCESS_MAIN, "EMERGENCY: Evacuation completed. All processes terminated.\n");
}

void start_perform_inventory() {
    send_log(PROCESS_MAIN, "=== INVENTORY REPORT ===\n");
    int total_remaining = 0;
    int total_produced = 0;
    int total_sold = 0;

    for (int i = 0; i < NUM_PRODUCTS; i++) {
        Product_dispenser *disp = &shared_memory->dispensers[i];
        total_remaining += disp->quantity;
        total_produced += disp->total_produced;
        total_sold += disp->total_sold;

        send_log(PROCESS_MAIN, "Dispenser %d:\n", i);
        send_log(PROCESS_MAIN, "  - Remaining: %d\n", disp->quantity);
        send_log(PROCESS_MAIN, "  - Total produced: %d\n", disp->total_produced);
        send_log(PROCESS_MAIN, "  - Total sold: %d\n", disp->total_sold);
    }

    send_log(PROCESS_MAIN, "\nSUMMARY:\n");
    send_log(PROCESS_MAIN, "Total remaining products: %d\n", total_remaining);
    send_log(PROCESS_MAIN, "Total produced products: %d\n", total_produced);
    send_log(PROCESS_MAIN, "Total sold products: %d\n", total_sold);
    send_log(PROCESS_MAIN, "=== END OF REPORT ===\n");
}

void stop_all_processes() {
    pid_t manager_pid = shared_memory->manager_pid;

    for (pid_t pid = 1; pid < 32768; pid++) {
        if (pid != manager_pid && kill(pid, 0) == 0) {
            kill(pid, SIGTERM);
        }
    }

    sleep(2);
}