#include "stdio.h"
#include <pthread.h>
#include <stdbool.h>
#include "unistd.h"
#include "../include/manager.h"

// funkcja otwarcia sklepu
//void open_shop(){
//    time_t now = time(NULL);
//    struct tm *current_time = localtime(&now);
//
//    if ((current_time->tm_hour < open_hours[0]) ||
//        (current_time->tm_hour == open_hours[0] && current_time->tm_min < open_hours[1]) ||
//        (current_time->tm_hour > close_hours[0]) ||
//        (current_time->tm_hour == close_hours[0] && current_time->tm_min > close_hours[1])) {
//        printf("Bakery is closed\n");
//        return;
//    }
//    else{
//
//        printf("Bakery is open\n");
//    }
//}

void open_shop() {
    pthread_mutex_lock(&g_mutex);
    g_storeOpen = true;
    pthread_cond_broadcast(&g_condStore);
    pthread_mutex_unlock(&g_mutex);
}

void close_shop() {
    pthread_mutex_lock(&g_mutex);
    g_storeOpen = false;
    pthread_mutex_unlock(&g_mutex);
}


void* manager_thread(void* arg) {
    int timeCounter = 0;

    while (1) {
        sleep(1);
        timeCounter++;
        printf("Manager work time: %d\n", timeCounter);

        if (timeCounter == 5) {
            open_shop();
        }

        if (timeCounter > 30) {
            close_shop();
            break;
        }
    }

    pthread_exit(NULL);
}

