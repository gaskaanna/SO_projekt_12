#include <stdbool.h>
#include <pthread.h>

#ifndef SO_PROJEKT_GLOBAL_H
#define SO_PROJEKT_GLOBAL_H

extern int g_currentClientsInStore;

extern int g_storeOpen;

extern pthread_mutex_t g_mutex;
extern pthread_cond_t g_condStore;
extern pthread_cond_t g_condCashier;

void wait_for_store_open();

#endif //SO_PROJEKT_GLOBAL_H
