# Producer and Consumer Problem Implementation
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
