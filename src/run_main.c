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
#define CLOSE_BAKERY_TIME 20
#define CLOSE_SIMULATION_TIME 25

int main(void) {
    signal(SIGINT, handle_signal);
    signal(SIGTSTP, handle_signal);
    signal(SIGTERM, handle_signal);

    init_logging("logs/bakery");

    my_pid = getpid();
    send_log(PROCESS_MAIN, "Main process started with PID: %d\n", my_pid);

    init_shared_memory();
    init_dispensers();
    init_baker();
    init_cashiers();

    shared_memory->manager_pid = my_pid;

    srand(time(NULL));
    g_currentTime = 0;

    while(1) {
        sleep(1);
        g_currentTime++;

        send_log(PROCESS_MAIN, "Current time: %d\n", g_currentTime);

        int new_clients = calculate_new_clients();
        init_new_clients(new_clients);

        if(g_currentTime == CLOSE_SIMULATION_TIME) {
            send_log(PROCESS_MAIN, "End of simulation. PID: %d\n", my_pid);
            stop_simulation();
            cleanup_memory();
            break;
        }

        if(g_currentTime == CLOSE_BAKERY_TIME) {
            send_log(PROCESS_MAIN, "Bakery is closing, performing inventory...\n");
            stop_all_processes();
            start_perform_inventory();
            continue;
        }

        if (rand() % 100000 == 0) start_evacuation();

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
    cleanup_logging();
    send_log(PROCESS_MAIN, "End of simulation. PID: %d\n", my_pid);
    return 0;
}