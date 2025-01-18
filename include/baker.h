#include <unistd.h>
#include "dispenser.h"

#ifndef SO_PROJEKT_BAKER_H
#define SO_PROJEKT_BAKER_H

int baker_exists; // zmienna globalna czy piekarz istnieje

typedef struct {
    pid_t id;
} Baker;

static Baker g_baker;

void create_baker();

#endif //SO_PROJEKT_BAKER_H
