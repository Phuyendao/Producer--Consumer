#include "shared.h"

int main() {
    printf("\n --- CONSUMER PROCESS STARTING --- \n");
    // Open the existing shared memory object
    int shared_mem_fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    if (shared_mem_fd == -1) {
        perror("Consumer: shm_open failed");
        exit(EXIT_FAILURE);
    }
    // Map the shared memory object into the process's address space
    SharedTable *shared_table = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_fd, 0);
    if (shared_table == MAP_FAILED) {
        perror("Consumer: mmap failed");
        exit(EXIT_FAILURE);
    }
    // Open the existing semaphores
    sem_t *full_sem = sem_open(SEM_FULL_NAME, O_CREAT, 0666, 0);
    sem_t *empty_sem = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, BUFFER_SIZE);
    sem_t *mutex_sem = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);

    if (full_sem == SEM_FAILED || empty_sem == SEM_FAILED || mutex_sem == SEM_FAILED) {
        perror("Consumer: sem_open failed");
        exit(EXIT_FAILURE);
    }
    // Seed random number generator for sleep times
    srand(time(NULL));

    const int total_items_to_consume = 10;
    int items_consumed = 0;

    while (items_consumed < total_items_to_consume) {
        // Wait for a filled slot in the table
        sem_wait(full_sem);

        // Enter critical section (lock the table)
        sem_wait(mutex_sem);

        // CRITICAL SECTION START
        // Consume an item
        int consumed_item = shared_table->items[shared_table->next_consume_index];

        printf("Consumer: Took item %d from position %d\n", 
               consumed_item, shared_table->next_consume_index);

        // Update the consume index (circular buffer)
        shared_table->next_consume_index = 
            (shared_table->next_consume_index + 1) % BUFFER_SIZE;

        items_consumed++;
        // CRITICAL SECTION END

        // Exit critical section and signal that a slot is now empty
        sem_post(mutex_sem);
        sem_post(empty_sem);

        // Simulate time to consume the item
        sleep(rand() % 3);
    }

    printf("--- CONSUMER PROCESS FINISHED ---\n");

    // Clean up resources
    sem_close(full_sem);
    sem_close(empty_sem);
    sem_close(mutex_sem);

    // Unlink semaphores and shared memory (last process to finish cleans up)
    sem_unlink(SEM_FULL_NAME);
    sem_unlink(SEM_EMPTY_NAME);
    sem_unlink(SEM_MUTEX_NAME);

    munmap(shared_table, SHARED_MEM_SIZE);
    close(shared_mem_fd);
    shm_unlink(SHARED_MEM_NAME);

    return EXIT_SUCCESS;
}