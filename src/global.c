#include <stdbool.h>
#include <pthread.h>
#include "../include/global.h"

bool g_storeOpen = false;

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  g_condStore  = PTHREAD_COND_INITIALIZER;
