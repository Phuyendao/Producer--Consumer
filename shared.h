#ifndef SHARED_H
#define SHARED_H

#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Constants for the shared buffer
#define BUFFER_SIZE 2  // Table can hold 2 items max

// Structure representing the shared table between producer and consumer
typedef struct {
    int items[BUFFER_SIZE];  // Buffer to store items
    int next_produce_index;  // Where the next item will be produced
    int next_consume_index;  // Where the next item will be consumed
} SharedTable;

// Shared memory configuration
#define SHARED_MEM_SIZE sizeof(SharedTable)
#define SHARED_MEM_NAME "/ProdConsumerTable"  // Name for shared memory

// Semaphore names
#define SEM_FULL_NAME  "/full_semaphore"   // Tracks filled slots
#define SEM_EMPTY_NAME "/empty_semaphore"  // Tracks empty slots
#define SEM_MUTEX_NAME "/table_mutex"      // Provides mutual exclusion

#endif