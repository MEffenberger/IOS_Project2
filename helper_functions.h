/**************************************************************
 *
 * Name of the Project: IOS Project 2, Process Synchronization
 * File: helper_functions.h
 * Created by: Marek Effenberger (xeffen00)
 * Description: This file contains declarations,
 *              libraries and constants
 *
 * ***********************************************************/

/**************************************************************
 *
 * @file helper_functions.h
 * @brief Header file for the project
 * @author Marek Effenberger
 *
 * ***********************************************************/

#ifndef IOS_PROJECT2_HELPER_FUNCTIONS_H
#define IOS_PROJECT2_HELPER_FUNCTIONS_H

//Libraries
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
#include <stdbool.h>

//Names of semaphores
#define SEM_PRINT_MUTEX "/xeffen00_print_mutex"
#define SEM_QUEUE1 "/xeffen00_queue1"
#define SEM_QUEUE2 "/xeffen00_queue2"
#define SEM_QUEUE3 "/xeffen00_queue3"
#define SEM_CLOSED "/xeffen00_closed"
#define SEM_USAGE_MUTEX "/xeffen00_usage_mutex"

//Semaphore declarations
sem_t *print_mutex;
sem_t *queue1;
sem_t *queue2;
sem_t *queue3;
sem_t *closed;
sem_t *usage_mutex;

//Shared usage_mutex declarations
int *process_counter;
int *closed_flag;
int *queue1_counter;
int *queue2_counter;
int *queue3_counter;

//File declaration
FILE *file;

//Structs of child processes
struct customer_args{
    int waiting_time;
    int customer_id;
};

struct officer_args{
    int break_time;
    int officer_id;
};

//Helper functions declarations
void error_message(int num);
void sharemem_init();
void semaphore_init();
void destroyer();
void waiting_generator(int waiting_time);
int queue_randomizer();
void my_print(int action, int id, int queue, char identifier, char *name);
int closing_time(int x);
void going_home(int customer_id, char identifier);
int choose_nonempty_queue();

#endif //IOS_PROJECT2_HELPER_FUNCTIONS_H
