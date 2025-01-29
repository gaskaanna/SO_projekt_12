#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "../include/shared_memory.h"
#include "../include/cashier.h"
#include "../include/queue_utils.h"
#include "../include/logs.h"
#include "../include/main.h"
int shmid;
struct shared_data *shared_memory;

void cleanup_memory() {
    if (shmid != -1) {
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl failed");
            exit(EXIT_FAILURE);
        }
    }
}

void handle_signal(int signum) {
    printf("\nReceived signal %d\n", signum);
    if (signum == SIGINT || signum == SIGTSTP) {
        send_log(PROCESS_MAIN, "Received %s signal. Starting evacuation...\n",
                signum == SIGINT ? "SIGINT" : "SIGTSTP");
        start_evacuation();
    }
    cleanup_memory();
    cleanup_logging();
    exit(signum);
}

void init_shared_memory() {
    shmid = shmget(SHM_KEY, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    shared_memory->manager_pid = getpid();
    shared_memory->baker_pid = 0;
    shared_memory->created_clients = 0;
    shared_memory->current_clients = 0;

    for (int i = 0; i < NUMBER_OF_CASHIERS; i++) {
        shared_memory->cashier_pids[i] = 0;
        shared_memory->cashier_active[i] = false;
        init_cashier_queue(&shared_memory->cashier_queues[i]);
        shared_memory->currently_served[i] = 0;
    }
}

void send_shutdown_signal(pid_t pid) {
    if (kill(pid, SIGTERM) == -1) {
        perror("kill failed");
    }
}