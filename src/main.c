#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/Product.h"
#include "../include/dispenser.h"


int main(void) {
    init_products();
    show_products();

    init_dispenser();
    show_dispenser();
    return 0;
}
