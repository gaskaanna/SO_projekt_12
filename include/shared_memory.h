#ifndef SO_PROJEKT_SHARED_MEMORY_H
#define SO_PROJEKT_SHARED_MEMORY_H

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "dispenser.h"
#include "cashier.h"

#define SHM_KEY 11231
#define MAX_LOG_MESSAGE 512

struct shared_data {
    pid_t manager_pid;
    pid_t baker_pid;
    pid_t cashier_pids[NUMBER_OF_CASHIERS];
    int log_pipe[2];
    int created_clients;
    int current_clients;
    bool cashier_active[NUMBER_OF_CASHIERS];
    Product_dispenser dispensers[NUM_PRODUCTS];
    CashierQueue cashier_queues[NUMBER_OF_CASHIERS];
    pid_t currently_served[NUMBER_OF_CASHIERS];
};

extern int shmid;
extern struct shared_data *shared_memory;

void init_shared_memory(void);
void cleanup_memory(void);
void handle_signal(int signum);
void send_shutdown_signal(pid_t pid);

#endif
