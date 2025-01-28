#ifndef SO_PROJEKT_MAIN_H
#define SO_PROJEKT_MAIN_H

extern pid_t my_pid;
extern int g_currentTime;
extern int created_clients;

int calculate_cashiers_needed(int currentClients);
int calculate_new_clients();
void activate_cashier(int cashier_id);
void deactivate_cashier(int cashier_id);
void stop_simulation();

#endif //SO_PROJEKT_MAIN_H