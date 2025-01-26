#include <stdbool.h>
#include <pthread.h>

#ifndef SO_PROJEKT_GLOBAL_H
#define SO_PROJEKT_GLOBAL_H

extern char g_logBasePath[256];
extern int g_timeCounter;
extern int g_currentClientsInStore;
extern int g_storeOpen;

extern FILE* g_mainLogFile;
extern pthread_mutex_t g_mainLogMutex;

void global_init_main_log(void);
void global_close_main_log(void);
void global_log_main(const char *format, ...);

extern pthread_mutex_t g_mutex;
extern pthread_cond_t g_condStore;
extern pthread_cond_t g_condCashier;

void wait_for_store_open();

#endif //SO_PROJEKT_GLOBAL_H
