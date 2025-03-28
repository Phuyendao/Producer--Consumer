#include "shared.h"

int main() {
    printf("\n--- PRODUCER PROCESS STARTING ---\n");

    // Create or open the shared memory object
    int shared_mem_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666); 
    if (shared_mem_fd == -1) {
        perror("Producer: shm_open failed");
        exit(EXIT_FAILURE);
    }
    // Set the size of the shared memory object
    if (ftruncate(shared_mem_fd, SHARED_MEM_SIZE) == -1) {
        perror("Producer: ftruncate failed");
        exit(EXIT_FAILURE);
    }
    // Map the shared memory object into the process's address space
    SharedTable *shared_table = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_fd, 0);
    if (shared_table == MAP_FAILED) {
        perror("Producer: mmap failed");
        exit(EXIT_FAILURE);
    }
    // Initialize shared table indices
    shared_table->next_produce_index = 0;
    shared_table->next_consume_index = 0;

    // Create/open semaphores
    sem_t *full_sem = sem_open(SEM_FULL_NAME, O_CREAT, 0666, 0);
    sem_t *empty_sem = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, BUFFER_SIZE);
    sem_t *mutex_sem = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);

    if (full_sem == SEM_FAILED || empty_sem == SEM_FAILED || mutex_sem == SEM_FAILED) {
        perror("Producer: sem_open failed");
        exit(EXIT_FAILURE);
    }

    // Seed random number generator for sleep times
    srand(time(NULL));

    const int total_items_to_produce = 10;
    int items_produced = 0;

    while (items_produced < total_items_to_produce) {
        sleep(rand() % 3); // Simulate time to produce an item
        sem_wait(empty_sem); // Wait for an empty slot in the table
        sem_wait(mutex_sem); // Enter critical section (lock the table)

        // CRITICAL SECTION START
        // Produce an item (random number between 0-99)
        int new_item = rand() % 100;
        shared_table->items[shared_table->next_produce_index] = new_item;

        printf("Producer: Placed item %d at position %d\n", 
               new_item, shared_table->next_produce_index);

        // Update the produce index (circular buffer)
        shared_table->next_produce_index = 
            (shared_table->next_produce_index + 1) % BUFFER_SIZE;

        items_produced++;    // CRITICAL SECTION END

        // Exit critical section and signal that a slot is now full
        sem_post(mutex_sem);
        sem_post(full_sem);
    }
    printf("---  PRODUCER PROCESS FINISHED ---\n");
    // Clean up resources
    sem_close(full_sem);
    sem_close(empty_sem);
    sem_close(mutex_sem);
    // The last process to finish should unlink (could be handled better)

    munmap(shared_table, SHARED_MEM_SIZE);
    close(shared_mem_fd);
    return EXIT_SUCCESS;
}
