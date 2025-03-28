# Producer and Consumer Problem Implementation
**Name** Phuong Huyen Dao

**Class** CS 33211 Operating system
## Overview
The Producer-Consumer problem is a classic synchronization problem:
- A producer process generates items and places them on a shared buffer (table)
- A consumer process removes items from the buffer
- The buffer has a limited capacity (2 items in this implementation)
- The producer must wait when the buffer is full, and the consumer must also wait when the buffer is empty

The implementation uses:
1. Shared memory for the buffer
2. Semaphores for synchronization
3. Mutual exclusion to protect critical sections

## Requirement
- Linux/Unix operating system
- GCC complier
- Semaphore and shared memory libraries

## GCC
```
gcc producer.c -pthread -lrt -o producer
gcc consumer.c -pthread -lrt -o consumer
./producer & ./consumer &
```
## Implementation details
### Semaphores
Three semaphores are used: 
1. *full_sem* : Counts number of filled slots (initialized to 0)
2. *empty_sem* : Counts number of empty slots (initialized to BUFFER_SIZE)
3. *mutex_sem* : Binary semaphore for mutual exclusion (initialized to 1)
### Producer
- Produces 10 items randomly
- Waits for the empty slot
- Locks mutex before accessing shared memory
- Places item in buffer
- Updates produce index
- Releases mutex and signals 
### Consumer
- Consumes 10 items
- Waits for filled slots
- Locks mutex before accessing shared memory
- Takes item from buffer
- Updates consume index
- Releases mutex and signals
### Shared memory 
- Defined in shared.h
- Contains a circular buffer of size 2
- Tracks next produce and consume indices
- Accessed by both producer and consumer

## Synchronization mechanism
The solution ensures:
1. Mutual exclusion:  Only one process accesses the buffer at a time
2. Bounded Waiting: Producers wait when the buffer is full, consumers wait when empty
3. Progress: Processes alternate access properly

The order of semaphore operations is crucial to prevent deadlock:
- Producer: wait(empty), wait(mutex), ... post(mutex), post(full)
- Consumer: wait(full), wait(mutex), ... post(mutex), post(empty)

## Sample output
![Sample output](https://github.com/user-attachments/assets/55ba4e99-989d-4f17-83ae-4f47dd3333f3)

## Cleanup
The consumer handles cleanup of:
- Semaphores (unlinking)
- Shared memory (unmapping and unlinking)

This ensures resources are properly released when the consumer finished 
