//
// Created by marek on 27.04.2023.
//

#ifndef IOS_PROJECT2_HELPER_FUNCTIONS_H
#define IOS_PROJECT2_HELPER_FUNCTIONS_H

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

#define SEM_MUTEX "/xeffen00_mutex"
#define SEM_OFFICER_AVAILABLE "/xeffen00_officer_available"
#define SEM_QUEUE1 "/xeffen00_queue1"
#define SEM_QUEUE2 "/xeffen00_queue2"
#define SEM_QUEUE3 "/xeffen00_queue3"
#define SEM_CLOSED "/xeffen00_closed"
#define QUEUES_MUTEX "/xeffen00_queues_mutex"
#define SEM_CUSTOMER_AVAILABLE "/xeffen00_customer_available"
#define SEM_CHOOSING_QUEUE "/xeffen00_choosing_queue"

sem_t *mutex;
sem_t *officer_available;
sem_t *customer_available;
sem_t *queue1;
sem_t *queue2;
sem_t *queue3;
sem_t *closed;
sem_t *queues_mutex;
sem_t *choosing_queue;

int *process_counter;
int *officer_counter;
int *customers_counter;
int *customers_number;
int *officers_number;
int *closed_flag;
int *queue1_counter;
int *queue2_counter;
int *queue3_counter;

FILE *file;

struct customer_args{
    //int state;
    int waiting_time;
    int customer_id;
};

struct officer_args{
    //int state;
    int break_time;
    int officer_id;
};


void error_message(int num);
void sharemem_init();
void semaphore_init();
void destroyer();
void waiting_generator(int waiting_time);
int random_1_to_3();
void my_print(int action, int id, int queue, char identifier, char *name);
int random_number(int x);
void going_home(int customer_id, char identifier);
int choose_nonempty_queue();

#endif //IOS_PROJECT2_HELPER_FUNCTIONS_H
