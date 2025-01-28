#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

#include "../include/shared_memory.h"
#include "../include/logs.h"
#include "../include/client.h"
#include "../include/queue_utils.h"
#include "../include/cashier.h"

#define SIGUSR1 10
#define SIGUSR2 12

volatile sig_atomic_t client_running = 1;

Client client_data;

void client_signal_handler(int signum) {
  printf("\n[CLIENT] Received signal %d. Preparing for shutdown...\n", signum);
  client_running = 0;
}

void cleanup_and_exit(CashierQueue *queue, int selected_cashier) {
    send_log(PROCESS_CLIENT, "Client %d was served by cashier %d and is leaving store\n",
             client_data.id, selected_cashier);
    shared_memory->current_clients--;
    close(shared_memory->log_pipe[0]);
    exit(EXIT_SUCCESS);
}

void init_client_data() {
    client_data.id = getpid();
    client_data.isShopping = true;

    int total_products_wanted = 0;

    for (int i = 0; i < NUM_PRODUCTS; i++) {
        client_data.shoppingList[i].productId = i;
        client_data.shoppingList[i].taken = 0;

        if (rand() % 2 == 1) {
            client_data.shoppingList[i].needed = (rand() % 5) + 1;
            total_products_wanted++;
        } else {
            client_data.shoppingList[i].needed = 0;
        }
    }

    if (total_products_wanted == 0) {
        int random_product = rand() % NUM_PRODUCTS;
        client_data.shoppingList[random_product].needed = (rand() % 5) + 1;
        total_products_wanted = 1;
    }

    send_log(PROCESS_CLIENT, "Client %d initialized, wants %d different products\n",
             client_data.id, total_products_wanted);

    for (int i = 0; i < NUM_PRODUCTS; i++) {
        if (client_data.shoppingList[i].needed > 0) {
            // send_log(PROCESS_CLIENT, "Client %d needs %d of product %d\n",
            //         client_data.id, client_data.shoppingList[i].needed, i);
        }
    }
}

void client_shopping() {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        if (client_data.shoppingList[i].needed > 0) {
            int needed = client_data.shoppingList[i].needed;
            int productId = client_data.shoppingList[i].productId;
            int quantity = 0;

            // Atomic operation to check and update quantity
            __sync_synchronize();  // Memory barrier
            int available = shared_memory->dispensers[productId].quantity;

            if (available < needed) {
                quantity = available;
                shared_memory->dispensers[productId].quantity = 0;
            } else {
                quantity = needed;
                shared_memory->dispensers[productId].quantity -= needed;
            }
            __sync_synchronize();  // Memory barrier

            client_data.shoppingList[i].taken = quantity;
        }
    }
}

int get_open_cashiers() {
    int open_cashiers = 0;
    for (int i = 0; i < NUMBER_OF_CASHIERS; i++) {
        if (shared_memory->cashier_active[i]) {
            open_cashiers++;
        }
    }
    return open_cashiers;
}

void wait_for_open_cashiers() {
    while (get_open_cashiers() == 0 && client_running) {
        send_log(PROCESS_CLIENT, "Client %d waiting for open cashier\n", client_data.id);
        sleep(1);
    }
}

void client_process() {
    signal(SIGTERM, client_signal_handler);
    signal(SIGUSR1, client_signal_handler);
    srand(time(NULL) ^ getpid());

    pid_t client_pid = getpid();
    send_log(PROCESS_CLIENT, "Client process started with PID: %d\n", client_pid);

    init_client_data();

    send_log(PROCESS_CLIENT, "Client %d is shopping\n", client_data.id);
    sleep(SHOPPING_TIME);
    client_shopping();

    wait_for_open_cashiers();

    // Wybór kasjera i dodanie do kolejki
    int selected_cashier = rand() % NUMBER_OF_CASHIERS;
    while (!shared_memory->cashier_active[selected_cashier]) {
        selected_cashier = (selected_cashier + 1) % NUMBER_OF_CASHIERS;
    }

    send_log(PROCESS_CLIENT, "Client %d selected cashier %d\n", client_data.id, selected_cashier);

    CashierQueue *selected_queue = &shared_memory->cashier_queues[selected_cashier];

    // Czekaj jeśli kolejka jest pełna
    while (is_queue_full(selected_queue) && client_running) {
        send_log(PROCESS_CLIENT, "Client %d waiting - queue %d is full\n", client_data.id, selected_cashier);
        sleep(1);
    }

    if (!client_running) return;

    // Dodaj do kolejki używając funkcji z queue_utils
    if (enqueue(selected_queue, client_pid)) {
        send_log(PROCESS_CLIENT, "Client %d joined queue for cashier %d at position %d\n",
                 client_data.id, selected_cashier,
                 get_queue_position(selected_queue, client_pid));
    }

    // Czekaj na obsłużenie
    while (client_running) {
        int position = get_queue_position(selected_queue, client_pid);
        if (position == -1) {  // No longer in queue
            cleanup_and_exit(selected_queue, selected_cashier);
            return;
        }
        if (position == 1) {
            send_log(PROCESS_CLIENT, "Client %d is first in line for cashier %d\n",
                     client_data.id, selected_cashier);
        }
        sleep(1);
    }

    cleanup_and_exit(selected_queue, selected_cashier);
}

void create_client() {
    pid_t client_pid = fork();

    if(client_pid < 0) {
        send_log(PROCESS_CLIENT, "Error creating client process\n");
        exit(EXIT_FAILURE);
    }
    if(client_pid == 0) {
        shared_memory->current_clients++;
        client_process();
        close(shared_memory->log_pipe[1]);  // Close write end
        close(shared_memory->log_pipe[0]);  // Close read end
        exit(EXIT_SUCCESS);
    }
}

void init_new_clients(int new_clients) {
  for (int i = 0; i < new_clients; i++) {
    create_client();
  }
}
