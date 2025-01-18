#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/Product.h"
#include "../include/dispenser.h"
#include "../include/client.h"
#include "../include/baker.h"

int main(void) {
    init_products();
    show_products();

    init_dispenser();
    show_dispenser();

    bake();
    bake();
    bake();
    return 0;
}
