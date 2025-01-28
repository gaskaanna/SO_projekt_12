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

int shmid;
struct shared_data *shared_memory;

void cleanup_memory() {
    close(shared_memory->log_pipe[0]);
    close(shared_memory->log_pipe[1]);

    if (shared_memory != NULL) {
        if (shmdt(shared_memory) == -1) {
            perror("shmdt");
        }
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    }
}

void init_shared_memory(){
    shmid = shmget(SHM_KEY, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return;
    }

    shared_memory = (struct shared_data *)shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL);
        return;
    }

    memset(shared_memory, 0, sizeof(struct shared_data));

    if (pipe(shared_memory->log_pipe) == -1) {
        perror("pipe");
        cleanup_memory();
        return;
    }

    int flags = fcntl(shared_memory->log_pipe[0], F_GETFL);
    fcntl(shared_memory->log_pipe[0], F_SETFL, flags | O_NONBLOCK);

    for (int i = 0; i < NUMBER_OF_CASHIERS; i++) {
        init_queue(&shared_memory->cashier_queues[i]);
        shared_memory->currently_served[i] = 0;
    }
}

void handle_signal(int signum) {
    printf("\n[MAIN] Received signal %d. Shutting down...\n", signum);

    if(signum == SIGINT) printf("SIGINT");
    if(signum == SIGTERM) printf("SIGTERM");

    cleanup_memory();
    exit(EXIT_SUCCESS);
}

void send_shutdown_signal(pid_t pid) {
    if (pid > 0) {
        kill(pid, SIGTERM);
    }
}