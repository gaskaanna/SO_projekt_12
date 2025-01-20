#include <stdio.h>
#include <time.h>
#include "global.h"

#ifndef SO_PROJEKT_MANAGER_H
#define SO_PROJEKT_MANAGER_H

#define MAX_CLIENTS_IN_STORE 30

void* manager_thread(void* arg);

#endif //SO_PROJEKT_MANAGER_H
