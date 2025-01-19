#include <stdbool.h>
#include <pthread.h>

#ifndef SO_PROJEKT_GLOBAL_H
#define SO_PROJEKT_GLOBAL_H

extern bool g_storeOpen;

extern pthread_mutex_t g_mutex;
extern pthread_cond_t  g_condStore;

#endif //SO_PROJEKT_GLOBAL_H
