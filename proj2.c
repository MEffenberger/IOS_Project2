#include "helper_functions.h"



void customer(int customer_id, int waiting_time){

    int queue = random_1_to_3();
    my_print(1, customer_id, 0, 'Z', "started");
    waiting_generator(waiting_time);

    sem_wait(memory);
    if(*closed_flag == 0){


    switch(queue){

        case 1:

            sem_wait(closed);
            going_home(customer_id, 'Z');
            sem_post(closed);
            (*queue1_counter)++;
            my_print(2, customer_id, queue, 'Z', "entering office for a service");
            sem_post(memory);

            sem_wait(queue1);


            my_print(1, customer_id, 0, 'Z', "called by office worker");
            waiting_generator(10);
            my_print(1, customer_id, 0, 'Z', "going home");
            destroyer();
            exit(0);

        case 2:

            sem_wait(closed);
            going_home(customer_id, 'Z');
            sem_post(closed);
            (*queue2_counter)++;
            my_print(2, customer_id, queue, 'Z', "entering office for a service");

            sem_post(memory);
            sem_wait(queue2);

            my_print(1, customer_id, 0, 'Z', "called by office worker");
            waiting_generator(10);


            my_print(1, customer_id, 0, 'Z', "going home");
            destroyer();
            exit(0);

        case 3:

            sem_wait(closed);
            going_home(customer_id, 'Z');
            sem_post(closed);
            (*queue3_counter)++;
            my_print(2, customer_id, queue, 'Z', "entering office for a service");

            sem_post(memory);
            sem_wait(queue3);

            my_print(1, customer_id, 0, 'Z', "called by office worker");
            waiting_generator(10);


            my_print(1, customer_id, 0, 'Z', "going home");
            destroyer();
            exit(0);

        }
    }
    else
    {
    sem_post(memory);
    }
        my_print(1, customer_id, 0, 'Z', "going home");
        destroyer();
        exit(0);
}



void officer(int officer_id, int break_time) {


    my_print(1, officer_id, 0, 'U', "started");

    while(1)
    {
        sem_wait(memory);
        if (*closed_flag == 1 && *queue1_counter == 0 && *queue2_counter == 0 && *queue3_counter == 0)
        {

            break;

        } else if (*queue1_counter > 0)
        {
                (*queue1_counter)--;
                my_print(2, officer_id, 1, 'U', "serving a service of type");
                sem_post(memory);
                sem_post(queue1);

                waiting_generator(10);

                my_print(1, officer_id, 0, 'U', "service finished");

        } else if (*queue2_counter > 0)
        {
                (*queue2_counter)--;
                my_print(2, officer_id, 2, 'U', "serving a service of type");
                sem_post(memory);
                sem_post(queue2);

                waiting_generator(10);

                my_print(1, officer_id, 0, 'U', "service finished");

        } else if (*queue3_counter > 0)
        {
                 (*queue3_counter)--;
                my_print(2, officer_id, 3, 'U', "serving a service of type");
                sem_post(memory);
                sem_post(queue3);

                waiting_generator(10);

                my_print(1, officer_id, 0, 'U', "service finished");


        } else
        {

            my_print(1, officer_id, 0, 'U', "taking break");
            sem_post(memory);
            waiting_generator(break_time);

            my_print(1, officer_id, 0, 'U', "break finished");
        }
    }

    sem_post(memory);
    my_print(1, officer_id, 0, 'U', "going home");
    destroyer();
    exit(0);

}



int main(int argc, char *argv[]) {

//**Argument parsing and error handling**//

    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_QUEUE1);
    sem_unlink(SEM_QUEUE2);
    sem_unlink(SEM_QUEUE3);
    sem_unlink(SEM_CLOSED);
    sem_unlink(SEM_MEMORY);
    sem_unlink(SEM_QUEUES_MUTEX);


    if ((file = fopen("proj2.out", "w")) == NULL)
    {
        error_message(4);
        return 1;
    }

    if (argc != 6)
    {
        error_message(1);
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        for (unsigned long int j = 0; j < strlen(argv[i]); j++)
        {
            if (argv[i][j] < '0' || argv[i][j] > '9')
            {
                error_message(2);
                return 1;
            }
        }
    }

    int CustomersNum = atoi(argv[1]);
    if (CustomersNum < 0)
    {
        error_message(3);
        return 1;
    }

    int OfficersNum = atoi(argv[2]);
    if (OfficersNum <= 0)
    {
        error_message(3);
        return 1;
    }

    int WaitingTime = atoi(argv[3]);
    if (WaitingTime < 0 || WaitingTime > 10000)
    {
        error_message(3);
        return 1;
    }

    int BreakTime = atoi(argv[4]);
    if (BreakTime < 0 || BreakTime > 100)
    {
        error_message(3);
        return 1;
    }

    int ClosedTime = atoi(argv[5]);
    if (ClosedTime < 0 || ClosedTime > 10000)
    {
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

    for (int i = 1; i <= CustomersNum; ++i)
    {
        customer_args_array[i].customer_id = i;
        customer_args_array[i].waiting_time = WaitingTime;
    }

    for (int i = 1; i <= OfficersNum; ++i)
    {
        officer_args_array[i].officer_id = i;
        officer_args_array[i].break_time = BreakTime;
    }


    for (int i = 1; i <= CustomersNum; ++i)
    {
        pid_t pid = fork();
        if (pid == 0)
        {

            customer(customer_args_array[i].customer_id, customer_args_array[i].waiting_time);
            exit(0);

        } else if (pid == -1)
        {
            error_message(4);
            destroyer();
            return 1;
        }
    }

    for (int i = 1; i <= OfficersNum; ++i)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            officer(officer_args_array[i].officer_id, officer_args_array[i].break_time);

            exit(0);
        } else if (pid == -1)
        {
            error_message(4);
            destroyer();
            return 1;
        }
    }

    //wait until all child processes are created


    usleep(random_number(ClosedTime) * 1000);
    sem_wait(memory);
    my_print(3, 0, 0, 0, "closing");
    (*closed_flag) = 1;
    sem_post(memory);


    while(wait(NULL) > 0);
    destroyer();
    return 0;

}