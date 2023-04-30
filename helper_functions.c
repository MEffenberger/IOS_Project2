#include "helper_functions.h"

/**
 * @brief Function for shared memory initialization
 */
void sharemem_init()
{

    process_counter = mmap(NULL, sizeof *process_counter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (process_counter == MAP_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }
    *process_counter = 0;


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
 * @brief Function for semaphore initialization
 */
void semaphore_init()
{

    mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1);

    if (mutex == SEM_FAILED){
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

    memory = sem_open(SEM_MEMORY, O_CREAT | O_EXCL, 0666, 1);

    if (memory == SEM_FAILED){
        error_message(6);
        destroyer();
        fclose(file);
        exit(1);
    }
}


/**
 * @brief Function for semaphore destruction, shared memory unmap and file close
 */
void destroyer()
{

    sem_unlink(SEM_MUTEX);
    sem_close(mutex);

    sem_unlink(SEM_QUEUE1);
    sem_close(queue1);

    sem_unlink(SEM_QUEUE2);
    sem_close(queue2);

    sem_unlink(SEM_QUEUE3);
    sem_close(queue3);

    sem_unlink(SEM_CLOSED);
    sem_close(closed);

    sem_unlink(SEM_MEMORY);
    sem_close(memory);



    munmap(closed_flag, sizeof *closed_flag);
    munmap(process_counter, sizeof *process_counter);
    munmap(queue1_counter, sizeof *queue1_counter);
    munmap(queue2_counter, sizeof *queue2_counter);
    munmap(queue3_counter, sizeof *queue3_counter);

    fclose(file);

}

/**
 * @brief Function for printing the output to the file
 * @param action For switch to know what to print based on the number of arguments
 * @param id The id of the CHILD process
 * @param queue The queue the customer chose
 * @param identifier The identifier of child process (Z for customer / U for officer)
 * @param name The actual name of the activity the child is doing
 */
void my_print(int action, int id, int queue, char identifier, char *name)
{
    sem_wait(mutex);
    (*process_counter)++;
    switch (action)
    {
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

/**
 * @brief Function for printing the error message to the stderr
 * @param num Number of the error message
 */
void error_message(int num)
{
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
        case 7:
            fprintf(stderr, "Error: Error creating child process");
            break;
    }
}

/**
 * @brief Function which usleeps the process for a random time
 * @param waiting_time Time of waiting of the caller
 */
void waiting_generator(int waiting_time){
    srand(time(NULL) * getpid());
    int random_number = rand() % (waiting_time+1);
    usleep(random_number * 1000);
}

/**
 * @brief Function which chooses a random queue for the customer
 * @return Number of the queue
 */
int queue_randomizer() {
    srand(time(NULL) * getpid()); // initialize random seed
    return rand() % 3 + 1; // return random number from 1 to 3
}

/**
 * @brief Function which generates a random closing time
 * @param ClosedTime Time after which the post office closes
 * @return The time in ms after which the post office closes
 */
int closing_time(int ClosedTime) {
    srand(time(NULL) * getpid());
    int result = rand() % ((ClosedTime/2) + 1) + (ClosedTime/2);
    return result;
}

/**
 * @brief Function for sending the customer home in specific edge cases
 * @param customer_id ID of the customer
 * @param identifier identifier of the child process
 */
void going_home(int customer_id, char identifier)
{
    if (*closed_flag == 1)
    {
        my_print(1, customer_id, 0, identifier, "going home");
        sem_post(closed);
        destroyer();
        exit(0);
    }
}

