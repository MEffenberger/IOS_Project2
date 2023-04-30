/**************************************************************
 *
 * Name of the Project: IOS Project 2, Process Synchronization
 * File: proj2.c
 * Created by: Marek Effenberger (xeffen00)
 * Description: This file contains the main logic of the project,
 *              handling of the input and output
 *
 * ***********************************************************/

/**************************************************************
 *
 * @file proj2.c
 * @brief Main logic of the project
 * @author Marek Effenberger
 *
 * ***********************************************************/

#include "helper_functions.h"

/**
 * Customer Function
 * @brief Function which represents a customer process
 * The customer starts and waits for a randomized time
 * If after the time passed is the post still open, he enters the office
 * and chooses a random queue to wait in. If not, he goes home.
 */
void customer(int customer_id, int waiting_time){

    //selection of a random queue to be seved in
    int queue = queue_randomizer();

    //starting and waiting before entering the office/ going home
    my_print(1, customer_id, 0, 'Z', "started");
    waiting_generator(waiting_time);

    //locking the usage_mutex for a specific task of a customer
    sem_wait(usage_mutex);

    if(*closed_flag == 0)
    {
        switch(queue){

        case 1:
            //checking if the office is closed, edge cases
            sem_wait(closed);
            going_home(customer_id, 'Z');
            sem_post(closed);
            //incrementing the queue counter so the officer knows how many people are waiting
            (*queue1_counter)++;
            my_print(2, customer_id, queue, 'Z', "entering office for a service");
            //unlocking the usage_mutex so the others can proceed
            sem_post(usage_mutex);

            //signalling the officer that the customer is waiting
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
            sem_post(usage_mutex);

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
            sem_post(usage_mutex);

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
    sem_post(usage_mutex);
    }
    //going home if the office is closed
    my_print(1, customer_id, 0, 'Z', "going home");
    destroyer();
    exit(0);
}


/**
 * Officer Function
 * @brief Function which represents a single process of an officer
 * Cycles before the closing time between serving customers and taking a break
 * After the closing time, the officer finishes serving and goes home
 */
void officer(int officer_id, int break_time) {

    //officer starts working
    my_print(1, officer_id, 0, 'U', "started");

    for(;;)
    {
        //locking usage_mutex for a specific task of an officer
        sem_wait(usage_mutex);
        if (*closed_flag == 1 && *queue1_counter == 0 && *queue2_counter == 0 && *queue3_counter == 0)
        {
            //breaking of the cycle if all customers are served, going home
            break;
        }
        //if there is a customer in a queue, the officer serves him, waits and then the cycle continues
        else if (*queue1_counter > 0 || *queue2_counter > 0 || *queue3_counter > 0){

            //choosing a nonempty random queue to serve
            int queue_to_serve = choose_nonempty_queue();
            if (queue_to_serve == 1)
            {
                //decrementing the queue counter so the officer knows how many people are waiting
                (*queue1_counter)--;
                my_print(2, officer_id, 1, 'U', "serving a service of type");
                //unlocking the usage_mutex so the others can proceed
                sem_post(usage_mutex);
                //signalling the customer that he is being served
                sem_post(queue1);

                waiting_generator(10);

                my_print(1, officer_id, 0, 'U', "service finished");

            } else if (queue_to_serve == 2)
            {
                (*queue2_counter)--;
                my_print(2, officer_id, 2, 'U', "serving a service of type");
                sem_post(usage_mutex);
                sem_post(queue2);

                waiting_generator(10);

                my_print(1, officer_id, 0, 'U', "service finished");

            } else if (queue_to_serve == 3)
            {
                (*queue3_counter)--;
                my_print(2, officer_id, 3, 'U', "serving a service of type");
                sem_post(usage_mutex);
                sem_post(queue3);

                waiting_generator(10);

                my_print(1, officer_id, 0, 'U', "service finished");

            }

        } else
        {
            // in every other instance (while the post is open) the officer takes a break
            my_print(1, officer_id, 0, 'U', "taking break");
            sem_post(usage_mutex);
            waiting_generator(break_time);

            my_print(1, officer_id, 0, 'U', "break finished");
        }
    }
    //going home
    sem_post(usage_mutex);
    my_print(1, officer_id, 0, 'U', "going home");
    destroyer();
    exit(0);

}


/**
 * Main Function
 * @brief Function responsible for argument parsing and creating processes
 * bearing responsibility for closing the post on the randomized time and
 * signalling the closing of the post to the other processes
 * @param argc Number of arguments
 * @param argv Arguments in format <number of customers> <number of officers> <waiting before entering> <max break time> <max opening time>
 * @return 0 if everything is ok, 1 if error occurs
 */
int main(int argc, char *argv[]) {

//-------------------- Argument parsing and error handling ----------------------//

    sem_unlink(SEM_PRINT_MUTEX);
    sem_unlink(SEM_QUEUE1);
    sem_unlink(SEM_QUEUE2);
    sem_unlink(SEM_QUEUE3);
    sem_unlink(SEM_CLOSED);
    sem_unlink(SEM_USAGE_MUTEX);

    //opening the file for writing
    if ((file = fopen("proj2.out", "w")) == NULL)
    {
        error_message(4);
        return 1;
    }

    //checking if the number of arguments is correct
    if (argc != 6)
    {
        error_message(1);
        fclose(file);
        return 1;
    }

    //checking if the arguments are numbers
    for (int i = 1; i < argc; i++)
    {
        for (unsigned long int j = 0; j < strlen(argv[i]); j++)
        {
            if (argv[i][j] < '0' || argv[i][j] > '9')
            {
                error_message(2);
                fclose(file);
                return 1;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////
    // loading the data into variables and checking the restrictions //
    ///////////////////////////////////////////////////////////////////
    int CustomersNum = atoi(argv[1]);
    if (CustomersNum < 0)
    {
        error_message(3);
        fclose(file);
        return 1;
    }

    int OfficersNum = atoi(argv[2]);
    if (OfficersNum <= 0)
    {
        error_message(3);
        fclose(file);
        return 1;
    }

    int WaitingTime = atoi(argv[3]);
    if (WaitingTime < 0 || WaitingTime > 10000)
    {
        error_message(3);
        fclose(file);
        return 1;
    }

    int BreakTime = atoi(argv[4]);
    if (BreakTime < 0 || BreakTime > 100)
    {
        error_message(3);
        fclose(file);
        return 1;
    }

    int ClosedTime = atoi(argv[5]);
    if (ClosedTime < 0 || ClosedTime > 10000)
    {
        error_message(3);
        fclose(file);
        return 1;
    }

//---------------------- Child processes creation and handling -------------------------//

    //setting up stdout, so the output is not buffered
    setbuf(stdout, NULL);

    //initializing semaphores and shared memory
    semaphore_init();
    sharemem_init();

    //creating array of structs to hold arguments for customers and officers
    struct customer_args customer_args_array[CustomersNum];
    struct officer_args officer_args_array[OfficersNum];

    //adding an id to each customer
    for (int i = 1; i <= CustomersNum; ++i)
    {
        customer_args_array[i].customer_id = i;
        customer_args_array[i].waiting_time = WaitingTime;
    }

    //adding an id to each officer
    for (int i = 1; i <= OfficersNum; ++i)
    {
        officer_args_array[i].officer_id = i;
        officer_args_array[i].break_time = BreakTime;
    }

    //forking customers
    for (int i = 1; i <= CustomersNum; ++i)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            customer(customer_args_array[i].customer_id, customer_args_array[i].waiting_time);
            exit(0);

        } else if (pid == -1)
        {
            error_message(7);
            destroyer();
            return 1;
        }
    }

    //forking officers
    for (int i = 1; i <= OfficersNum; ++i)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            officer(officer_args_array[i].officer_id, officer_args_array[i].break_time);
            exit(0);

        } else if (pid == -1)
        {
            error_message(7);
            destroyer();
            return 1;
        }
    }

    //called usleep in a range of <F/2,F> print_mutex lock needed for collision prevention, setting flag to 1
    usleep(closing_time(ClosedTime) * 1000);
    sem_wait(usage_mutex);
    my_print(3, 0, 0, 0, "closing");
    (*closed_flag) = 1;
    sem_post(usage_mutex);
    sem_post(closed);

    //waiting for all processes to finish, cleaning main afterward
    while(wait(NULL) > 0);
    destroyer();
    return 0;
}