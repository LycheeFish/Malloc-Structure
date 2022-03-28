#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

#define MEMSIZE 4096

static char memory[MEMSIZE];

// debugging use
void print_all_blocks() {
    char *searcher = &memory[sizeof(int) + 1];
    printf("blocks: ");
    while (searcher < memory + MEMSIZE) {
        int payload_size = *(int *)(searcher - 1 - sizeof(int));
        char is_used = *(searcher - 1);
        printf("(%d,%d)", payload_size, is_used);
        searcher += payload_size + sizeof(int) + 1;
    }
    printf("\n");
}

void *mymalloc(size_t size, char *file, int line) {
    printf("mymalloc called from %s:%d, block size %ld\n", file, line, size);

    // check if memory is uninitialized
    char initialized = '\0';
    for (int i = 0; i < MEMSIZE; i++) {
        if (memory[i] != 0) {
            initialized = '\1';
            break;
        }
    }
    if (!initialized) {
        printf("initializing memory (sizeof(int) = %lu)\n", sizeof(int));
        int *metadata_ptr = (int *)&memory[0];
        *metadata_ptr = MEMSIZE - sizeof(int) - 1;
        // memory[sizeof(int)] is already 0 (== data is free), so no need to set it
    }

    char *searcher = &memory[sizeof(int) + 1];
    while (searcher < memory + MEMSIZE) {
        int payload_size = *(int *)(searcher - 1 - sizeof(int));
        char is_used = *(searcher - 1);
        // size < payload_size - sizeof(int) - 1, to account for 
        // the space needed to make the new metadata
        if (!is_used && size <= payload_size - sizeof(int) - 1) {
            *(int *)(searcher - 1 - sizeof(int)) = size;
            *(searcher - 1) = '\1';

            searcher += size;
            // make a new metadata here
            *(int *)(searcher) = payload_size - size - sizeof(int) - 1;
            *(searcher + sizeof(int)) = '\0';

            print_all_blocks();

            return searcher + sizeof(int) + 1;
        }
        else {
            searcher += payload_size + sizeof(int) + 1;
        }
    }
    
    printf("failed to find an available block!\n");
    
    return NULL;
}

void myfree(void *ptr, char *file, int line) {
    printf("free called from %s:%d\n", file, line);

    // 1. check if ptr is inside memory range
    if ((char *)ptr < memory || (char *)ptr > &memory[MEMSIZE - 1]) {
        printf("given address of data of pointer is out of memory range!\n");
        return;
    }

    // 2. check if ptr is pointing to start of data chunk
    char found = '\0';
    char *searcher = &memory[sizeof(int) + 1];
    while (searcher < memory + MEMSIZE) {
        if (searcher == ptr) {
            found = '\1';
            // 3. check if data is not already free
            if (*(searcher - 1) == '\0') {
                printf("this data chunk is already free!\n");
                return;
            }
            break;
        }
        int payload_size = *(int *)(searcher - 1 - sizeof(int));
        searcher += payload_size + sizeof(int) + 1;
    }
    if (!found) {
        printf("address does not start at beginning of chunk!\n");
        return;
    }
}