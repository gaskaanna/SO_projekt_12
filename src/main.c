#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>
#include <pthread.h>
#include "../include/global.h"
#include "../include/manager.h"
#include "../include/baker.h"
#include "../include/cashier.h"

int main(void) {
    srand(time(NULL));
    time_t now = time(NULL);
    struct tm* tinfo = localtime(&now);

    strftime(g_logBasePath, sizeof(g_logBasePath), "../logs/%d_%m_%Y_%H-%M-%S", tinfo);

    global_init_main_log();

    pthread_t manager;
    pthread_t baker;
    pthread_t cashiers[NUM_CASHIERS];

    init_dispensers();

    pthread_create(&manager, NULL, manager_thread, NULL);
    pthread_create(&baker, NULL, baker_thread, NULL);

    init_cashiers(cashiers);
    init_clients();

    for (int i = 0; i < NUM_CASHIERS; i++) {
        printf("[MAIN] Joining cashier %d\n", i);
        pthread_join(cashiers[i], NULL);
    }

    pthread_join(manager, NULL);
    pthread_join(baker, NULL);


    printf("[MAIN] End of symulation.\n");
    return 0;
}
