#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#include "../include/shared_memory.h"
#include "../include/main.h"
#include "../include/cashier.h"

#define MIN_INIT_CLIENTS 5
#define MAX_INIT_CLIENTS 10

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
