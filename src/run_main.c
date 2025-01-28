#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "../include/shared_memory.h"
#include "../include/logs.h"
#include "../include/baker.h"
#include "../include/cashier.h"
#include "../include/dispenser.h"
#include "../include/main.h"
#include "../include/client.h"

#define OPEN_BAKERY_TIME 5
#define CLOSE_BAKERY_TIME 180

int main(void) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    my_pid = getpid();
    printf("[MAIN] Main process started with PID: %d\n", my_pid);

    init_shared_memory();
    shared_memory->manager_pid = getpid();

    init_dispensers();
    init_baker();
    init_cashiers();

    while(1) {
        sleep(1);
        g_currentTime++;
        printf("[MAIN] Current time: %d\n", g_currentTime);
        read_log();

        int new_clients = calculate_new_clients();
        init_new_clients(new_clients);

        if(g_currentTime == CLOSE_BAKERY_TIME) {
            printf("[MAIN] End of simulation. PID: %d\n", my_pid);
            stop_simulation();
            cleanup_memory();
            break;
        }

        int cashiers_needed = calculate_cashiers_needed(shared_memory->current_clients);

        for(int i = 0; i < NUMBER_OF_CASHIERS; i++) {
            if (i < cashiers_needed) {
                activate_cashier(i);
            } else {
                deactivate_cashier(i);
            }
        }
    }

    cleanup_memory();
    printf("[MAIN] End of simulation. PID: %d\n", my_pid);
    return 0;
}