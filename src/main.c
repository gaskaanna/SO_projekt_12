#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include "../include/manager.h"
#include "../include/baker.h"
#include "../include/cashier.h"
#include "../include/client.h"

int main(void) {
    srand(time(NULL));

    pthread_t manager;
    pthread_t baker;

    init_dispensers();

    pthread_create(&manager, NULL, manager_thread, NULL);
    pthread_create(&baker, NULL, baker_thread, NULL);

    init_cashiers();
    init_clients();

    pthread_join(manager, NULL);
    pthread_join(baker, NULL);

    printf("[MAIN] End of symulation.\n");
    return 0;
}
