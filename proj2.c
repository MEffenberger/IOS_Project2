#include "helper_functions.h"



void customer(int customer_id, int waiting_time){

    char identifier = 'Z';

    my_print(1, customer_id, 0, 'Z', "started");
    waiting_generator(waiting_time);

//    sem_wait(closed);
//    going_home(customer_id, identifier);
//    sem_post(closed);

    int queue = random_1_to_3();
    switch(queue){

        case 1:

            sem_wait(closed);
            going_home(customer_id, identifier);
            sem_post(closed);

            my_print(2, customer_id, queue, identifier, "entering office for a service");
            sem_wait(queues_mutex);
            (*queue1_counter)++;
            sem_post(queue1);
            sem_post(queues_mutex);
            break;

        case 2:

            sem_wait(closed);
            going_home(customer_id, identifier);
            sem_post(closed);

            my_print(2, customer_id, queue, identifier, "entering office for a service");
            sem_wait(queues_mutex);
            (*queue2_counter)++;
            sem_post(queue2);
            sem_post(queues_mutex);
            break;

        case 3:

            sem_wait(closed);
            going_home(customer_id, identifier);
            sem_post(closed);

            my_print(2, customer_id, queue, identifier, "entering office for a service");
            sem_wait(queues_mutex);
            (*queue3_counter)++;
            sem_post(queue3);
            sem_post(queues_mutex);
            break;

    }

//    sem_post(customer_available);
//    sem_wait(officer_available);

   // my_print(1, customer_id, queue, identifier, "called by office worker");
//
   // waiting_generator(10);
//
    //my_print(1, customer_id, 0, identifier, "going home");

   exit(0);





}


void officer(int officer_id, int break_time) {

    char identifier = 'U';
    my_print(1, officer_id, 0, 'U', "started");

//    (*officer_counter)++;


    while(1) {

        if (*closed_flag == 1 && *queue1_counter == 0 && *queue2_counter == 0 && *queue3_counter == 0) {

            sem_wait(closed);
            going_home(officer_id, identifier);
            sem_post(closed);
            waiting_generator(break_time);
            exit(0);

        } else if (*queue1_counter > 0) {
                sem_wait(queue1);
                (*queue1_counter)--;
                my_print(2, officer_id, 1, 'U', "serving a service of type");
                waiting_generator(10);
                my_print(1, officer_id, 0, 'U', "service finished");
        } else if (*queue2_counter > 0) {
                sem_wait(queue2);
                (*queue2_counter)--;
                my_print(2, officer_id, 2, 'U', "serving a service of type");
                waiting_generator(10);
                my_print(1, officer_id, 0, 'U', "service finished");
        } else if (*queue3_counter > 0) {
                sem_wait(queue3);
                (*queue3_counter)--;
                my_print(2, officer_id, 3, 'U', "serving a service of type");
                waiting_generator(10);
                my_print(1, officer_id, 0, 'U', "service finished");
        }



    }
}



int main(int argc, char *argv[]) {

//**Argument parsing and error handling**//

    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_OFFICER_AVAILABLE);
    sem_unlink(SEM_QUEUE1);
    sem_unlink(SEM_QUEUE2);
    sem_unlink(SEM_QUEUE3);
    sem_unlink(SEM_CLOSED);
    sem_unlink(QUEUES_MUTEX);
    sem_unlink(SEM_CUSTOMER_AVAILABLE);
    sem_unlink(SEM_CHOOSING_QUEUE);


    if ((file = fopen("proj2.out", "w")) == NULL) {
        error_message(4);
        return 1;
    }

    if (argc != 6) {
        error_message(1);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        for (unsigned long int j = 0; j < strlen(argv[i]); j++) {
            if (argv[i][j] < '0' || argv[i][j] > '9') {
                error_message(2);
                return 1;
            }
        }
    }

    int CustomersNum = atoi(argv[1]);
    int OfficersNum = atoi(argv[2]);

    int WaitingTime = atoi(argv[3]);
    if (WaitingTime < 0 || WaitingTime > 10000) {
        error_message(3);
        return 1;
    }

    int BreakTime = atoi(argv[4]);
    if (BreakTime < 0 || BreakTime > 100) {
        error_message(3);
        return 1;
    }

    int ClosedTime = atoi(argv[5]);
    if (ClosedTime < 0 || ClosedTime > 10000) {
        error_message(3);
        return 1;
    }

//******************************************//

    setbuf(stdout, NULL);

    semaphore_init();
    sharemem_init();


    *customers_number = CustomersNum;
    *officers_number = OfficersNum;

    struct customer_args customer_args_array[CustomersNum];
    struct officer_args officer_args_array[OfficersNum];

    for (int i = 1; i <= CustomersNum; ++i) {
        customer_args_array[i].customer_id = i;
        customer_args_array[i].waiting_time = WaitingTime;
    }

    for (int i = 1; i <= OfficersNum; ++i) {
        officer_args_array[i].officer_id = i;
        officer_args_array[i].break_time = BreakTime;
    }


    for (int i = 1; i <= CustomersNum; ++i) {
        pid_t pid = fork();
        if (pid == 0) {

            customer(customer_args_array[i].customer_id, customer_args_array[i].waiting_time);

            exit(0);
        } else if (pid == -1) {
            error_message(4);
            return 1;
        }
    }

    for (int i = 1; i <= OfficersNum; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            officer(officer_args_array[i].officer_id, officer_args_array[i].break_time);

            exit(0);
        } else if (pid == -1) {
            error_message(4);
            return 1;
        }
    }



    usleep(random_number(ClosedTime) * 1000);
    sem_wait(closed);
    my_print(3, 0, 0, 0, "closing");
    (*closed_flag) = 1;
    sem_post(closed);


    while(wait(NULL) > 0);
    destroyer();
    return 0;

}