#include "stdio.h"
#include <pthread.h>
#include "../include/manager.h"
#include "../include/baker.h"
#include "../include/cashier.h"

int main(void) {
    pthread_t manager;
    pthread_t baker;

    init_dispensers();

    pthread_create(&manager, NULL, manager_thread, NULL);
    pthread_create(&baker, NULL, baker_thread, NULL);

    init_cashiers();

    pthread_join(manager, NULL);
    pthread_join(baker, NULL);

    printf("[MAIN] Koniec symulacji.\n");
    return 0;
}
