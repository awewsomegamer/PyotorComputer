#include "include/shared_memory.h"
#include "include/global.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdatomic.h>

#define BACKING_FILE "Data"

uint8_t *memory = NULL;
int fd = 0;
atomic_char16_t *lock;

void init_shared_memory() {
	DBG(1, printf("Initializing shared memory");)

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

	// lock = (atomic_char16_t *)(BUFFER_SIZE - 2);

	DBG(1, printf("Initialized shared memory");)
}

void destroy_shared_memory() {
        DBG(1, printf("Destroying shared memory");)

        munmap(memory, BUFFER_SIZE);
        close(fd);
        shm_unlink(BACKING_FILE);

        DBG(1, printf("Destroyed shared memory");)
}

void acquire_lock() {
	while (atomic_load_explicit(lock, memory_order_acquire) == 1);
	atomic_store_explicit(lock, 1, memory_order_relaxed);
}

void release_lock() {
	atomic_store_explicit(lock, 0, memory_order_release);
}