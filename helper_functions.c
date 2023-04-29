#include "helper_functions.h"


void waiting_generator(int waiting_time){

    srand(time(NULL) * getpid());
    int random_number = rand() % waiting_time;
    usleep(random_number * 1000);

}

int random_1_to_3() {
    srand(time(NULL) * getpid()); // initialize random seed
    return rand() % 3 + 1; // return random number from 1 to 3
}

int random_number(int x) {
    srand(time(NULL) * getpid());
    int result = rand() % ((x+1) - (x/2)) + (x/2);
    return result;
}

bool are_queues_empty() {

    sem_wait(queues_mutex);
    if (*queue1_counter == 0 && *queue2_counter == 0 && *queue3_counter == 0) {
        sem_post(queues_mutex);
        return true;

    }
    sem_post(queues_mutex);
    return false;

}

/**
 * @brief Funkce pro výpis chybové hlášky
 * @param num Číslo chyby
 */
void error_message(int num){
    switch(num){
        case 1:
            fprintf(stderr, "Error: Wrong number of arguments");
            break;
        case 2:
            fprintf(stderr, "Error: Wrong argument format");
            break;
        case 3:
            fprintf(stderr, "Error: Argument value out of range");
            break;
        case 4:
            fprintf(stderr, "Error: Error opening file");
            break;
        case 5:
            fprintf(stderr, "Error: Error creating semaphore");
            break;
        case 6:
            fprintf(stderr, "Error: Error opening shared memory");
            break;
    }
}


/**
 * @brief Funkce pro inicializaci sdílené paměti
 */
void sharemem_init(){

    process_counter = mmap(NULL, sizeof *process_counter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (process_counter == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }
    *process_counter = 0;

    officer_counter = mmap(NULL, sizeof *officer_counter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (officer_counter == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }
    *officer_counter = 0;

    customers_counter = mmap(NULL, sizeof *customers_counter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (customers_counter == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }
    *customers_counter = 0;

    customers_number = mmap(NULL, sizeof *customers_number, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (customers_number == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }
    *customers_number = 0;

    officers_number = mmap(NULL, sizeof *officers_number, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (officers_number == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }
    *officers_number = 0;

    closed_flag = mmap(NULL, sizeof *closed_flag, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (closed_flag == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }
    *closed_flag = 0;

    queue1_counter = mmap(NULL, sizeof *queue1_counter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (queue1_counter == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }

    *queue1_counter = 0;

    queue2_counter = mmap(NULL, sizeof *queue2_counter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (queue2_counter == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }

    *queue2_counter = 0;

    queue3_counter = mmap(NULL, sizeof *queue3_counter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (queue3_counter == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }

    *queue3_counter = 0;

}


/**
 * @brief Funkce pro inicializaci semaforů
 */
void semaphore_init(){

    mutex = NULL;
    officer_available = NULL;
    queue1 = NULL;
    queue2 = NULL;
    queue3 = NULL;
    closed = NULL;

    mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1);

    if (mutex == SEM_FAILED){
        error_message(5);
        destroyer();
        fclose(file);
        exit(1);
    }


    officer_available = sem_open(SEM_OFFICER_AVAILABLE, O_CREAT | O_EXCL, 0666, 0);

    if (officer_available == SEM_FAILED){
        error_message(5);
        destroyer();
        fclose(file);
        exit(1);
    }

    customer_available = sem_open(SEM_CUSTOMER_AVAILABLE, O_CREAT | O_EXCL, 0666, 0);

    if (customer_available == SEM_FAILED){
        error_message(5);
        destroyer();
        fclose(file);
        exit(1);
    }

    queue1 = sem_open(SEM_QUEUE1, O_CREAT | O_EXCL, 0666, 0);

    if (queue1 == SEM_FAILED){
        error_message(5);
        destroyer();
        fclose(file);
        exit(1);
    }

    queue2 = sem_open(SEM_QUEUE2, O_CREAT | O_EXCL, 0666, 0);

    if (queue2 == SEM_FAILED){
        error_message(5);
        destroyer();
        fclose(file);
        exit(1);
    }

    queue3 = sem_open(SEM_QUEUE3, O_CREAT | O_EXCL, 0666, 0);

    if (queue3 == SEM_FAILED){
        error_message(5);
        destroyer();
        fclose(file);
        exit(1);
    }

    closed = sem_open(SEM_CLOSED, O_CREAT | O_EXCL, 0666, 1);

    if (closed == SEM_FAILED){
        error_message(5);
        destroyer();
        fclose(file);
        exit(1);
    }

    queues_mutex = sem_open(QUEUES_MUTEX, O_CREAT | O_EXCL, 0666, 1);

    if (queues_mutex == SEM_FAILED){
        error_message(5);
        destroyer();
        fclose(file);
        exit(1);
    }

    choosing_queue = sem_open(SEM_CHOOSING_QUEUE, O_CREAT | O_EXCL, 0666, 1);

    if (choosing_queue == SEM_FAILED){
        error_message(5);
        destroyer();
        fclose(file);
        exit(1);
    }
}


/**
 * @brief Funkce uvolňující všechny semafory a sdílenou paměť
 */
void destroyer(){

    sem_unlink(SEM_MUTEX);
    sem_close(mutex);

    sem_unlink(SEM_OFFICER_AVAILABLE);
    sem_close(officer_available);

    sem_unlink(SEM_QUEUE1);
    sem_close(queue1);

    sem_unlink(SEM_QUEUE2);
    sem_close(queue2);

    sem_unlink(SEM_QUEUE3);
    sem_close(queue3);

    sem_unlink(SEM_CLOSED);
    sem_close(closed);

    sem_unlink(QUEUES_MUTEX);
    sem_close(queues_mutex);

    sem_unlink(SEM_CUSTOMER_AVAILABLE);
    sem_close(customer_available);

    sem_unlink(SEM_CHOOSING_QUEUE);
    sem_close(choosing_queue);

    munmap(closed_flag, sizeof *closed_flag);
    munmap(process_counter, sizeof *process_counter);
    munmap(officer_counter, sizeof *officer_counter);
    munmap(customers_counter, sizeof *customers_counter);
    munmap(customers_number, sizeof *customers_number);
    munmap(officers_number, sizeof *officers_number);
    munmap(queue1_counter, sizeof *queue1_counter);
    munmap(queue2_counter, sizeof *queue2_counter);
    munmap(queue3_counter, sizeof *queue3_counter);

    fclose(file);

}

void my_print(int action, int id, int queue, char identifier, char *name){

    sem_wait(mutex);
    (*process_counter)++;
    switch (action){
        case 1:
            fprintf(file, "%d: %c %d: %s\n", *process_counter, identifier, id, name);
            break;
        case 2:
            fprintf(file, "%d: %c %d: %s %d\n", *process_counter, identifier, id, name ,queue);
            break;
        case 3:
            fprintf(file, "%d: %s\n", *process_counter, name);
            break;
    }
    fflush(file);
    sem_post(mutex);
}

void going_home(int customer_id, char identifier){
    if (*closed_flag == 1){
        my_print(1, customer_id, 0, identifier, "going home");
        (*customers_counter)++;
        sem_post(closed);
        //fclose(file);
        destroyer();
        exit(0);
    }
}

