#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include "../include/Product.h"
#include "../include/dispenser.h"
#include "../include/baker.h"
#include "../include/cashier.h"

int main(void) {
    init_products();
//    show_products();
//
    init_dispenser();
//    show_dispenser();

    create_baker();
    create_baker(); //testowanie sprawdzenie czy istnieje piekarz i czy utworzy się nowy proces

    if (baker_exists) {
        waitpid(g_baker.id, NULL, 0);
    }

    return 0;
}
