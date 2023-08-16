#include "include/shared_memory.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdatomic.h>

#define BACKING_FILE "Data"

uint8_t *memory = NULL;
int fd = 0;
atomic_flag *lock;
uint8_t lock_owned = 0;

void init_shared_memory_host() {
	DBG(1, printf("Initializing shared memory host");)

        fd = shm_open(BACKING_FILE, O_RDWR | O_CREAT, 0644);
        
        if (fd < 0) {
                printf("Failed to initialize memory\n");
                exit(1);
        }

        ftruncate(fd, BUFFER_SIZE);
        memory = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if (((uint8_t *) -1) == memory) {
                printf("Failed to map memory\n");
                exit(1);
        }

        lock = (atomic_flag *)(memory + BUFFER_SIZE - 1);

	DBG(1, printf("Initialized shared memory host");)
}

void init_shared_memory_client() {
        DBG(1, printf("Initializing shared memory client");)

        fd = shm_open(BACKING_FILE, O_RDWR, 0644);

        if (fd < 0) {
                printf("Failed to initialize memory\n");
                exit(1);
        }

        memory = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if (((uint8_t *) -1) == memory) {
                printf("Failed to map memory\n");
                exit(1);
        }

        lock = (atomic_flag *)(memory + BUFFER_SIZE - 1);

        DBG(1, printf("Initialized shared memory client");)
}

void destroy_shared_memory() {
        DBG(1, printf("Destroying shared memory host");)

        munmap(memory, BUFFER_SIZE);
        close(fd);
        shm_unlink(BACKING_FILE);

        DBG(1, printf("Destroyed shared memory host");)
}

void shared_memory_acquire_lock() {
        while (atomic_flag_test_and_set_explicit(lock, memory_order_acquire))
                __builtin_ia32_pause();
        lock_owned = 1;                
}

void shared_memory_release_lock() {
        atomic_flag_clear_explicit(lock, memory_order_release);
        lock_owned = 0;
}