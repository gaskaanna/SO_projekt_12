#ifndef MAIN_H
#define MAIN_H

#include <time.h>

extern pid_t my_pid;
extern int g_currentTime;
extern int created_clients;

int calculate_cashiers_needed(int currentClients);
int calculate_new_clients(void);
void activate_cashier(int cashier_id);
void deactivate_cashier(int cashier_id);
void stop_simulation(void);
void start_evacuation(void);
void start_perform_inventory(void);
void stop_all_processes(void);

#endif //SO_PROJEKT_MAIN_H