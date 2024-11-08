#include "ceit.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/** Global pointer to the head of the Memchunk list. */
Memchunk* global_memchunk_list = NULL;

/**
 * @brief Initializes a new memory Memchunk with the given name and total size.
 * 
 * This function allocates memory for the Memchunk structure and initializes
 * the memory pool. Initially, all memory in the Memchunk is free.
 * 
 * @param name The name of the Memchunk to initialize.
 * @param total_size The total size of the memory Memchunk.
 * 
 * @return A pointer to the initialized Memchunk structure, or NULL if memory 
 *         allocation fails.
 * 
 * Example usage:
 * ```
 * Memchunk* chunk = memc_init("Chunk1", 1024);
 * if (chunk) {
 *     // Use the chunk
 * }
 * ```
 */
Memchunk* memc_init(const char* name, size_t total_size) {
    Memchunk* new_Memchunk = (Memchunk*)malloc(sizeof(Memchunk));
    if (new_Memchunk == NULL) return NULL;

    // Ensure the Memchunk name is properly set and null-terminated
    strncpy(new_Memchunk->name, name, sizeof(new_Memchunk->name));
    new_Memchunk->name[sizeof(new_Memchunk->name) - 1] = '\0';  // Null-terminate the name string

    new_Memchunk->total_size = total_size;
    new_Memchunk->used_memory = 0;  // Initially no memory is used
    new_Memchunk->free_memory = total_size;  // All memory is free at the start

    new_Memchunk->memory_pool = (Memory*)malloc(total_size + sizeof(Memory));
    if (new_Memchunk->memory_pool == NULL) return NULL;

    new_Memchunk->memory_pool->size = total_size;
    new_Memchunk->memory_pool->is_free = 1;  // Initially, the memory is free
    new_Memchunk->memory_pool->next = NULL;
    new_Memchunk->next = NULL;

    return new_Memchunk;
}

/**
 * @brief Allocates memory from the Memchunk's memory pool.
 * 
 * This function searches for the best-fit memory block in the pool that is large
 * enough to satisfy the request and allocates it. If the block is larger than 
 * necessary, it is split. The allocated memory block is marked as used, and 
 * the Memchunk's used/free memory statistics are updated.
 * 
 * @param Memchunk The Memchunk from which memory is allocated.
 * @param size The size of the memory block to allocate.
 * @param block_name The name of the allocated memory block.
 * 
 * @return A pointer to the allocated memory block, or NULL if allocation fails.
 * 
 * Example usage:
 * ```
 * void* allocated_memory = memory_alloc(chunk, 256, "Block1");
 * if (allocated_memory) {
 *     // Use the allocated memory
 * }
 * ```
 */
void* memory_alloc(Memchunk* Memchunk, size_t size, const char* block_name) {
    if (!Memchunk || size == 0) return NULL;

    Memory* current = Memchunk->memory_pool, *best_fit = NULL;
    size_t best_fit_size = (size_t)-1;
    while (current) {
        if (current->is_free && current->size >= size && current->size < best_fit_size) {
            best_fit = current;
            best_fit_size = current->size;
        }
        current = current->next;
    }

    if (!best_fit) return NULL;  // No suitable memory block found

    // Split the memory block if the remaining space is large enough
    if (best_fit->size > size + sizeof(Memory)) {
        Memory* new_block = (Memory*)((char*)best_fit + sizeof(Memory) + size);
        new_block->size = best_fit->size - size - sizeof(Memory);
        new_block->is_free = 1;
        new_block->next = best_fit->next;
        best_fit->size = size;
        best_fit->next = new_block;
    }

    best_fit->is_free = 0;  // Mark the block as used
    strncpy(best_fit->name, block_name, sizeof(best_fit->name));

    // Update Memchunk's used and free memory
    Memchunk->used_memory += size;
    Memchunk->free_memory -= size;

    return (void*)((char*)best_fit + sizeof(Memory));  // Return the memory block's data pointer
}

/**
 * @brief Writes data to the allocated memory block.
 * 
 * This function writes data to the memory block. If the `size` is 0, it will 
 * automatically calculate the size based on the type of data. For example, 
 * if the data is a string, it will calculate the length of the string.
 * 
 * @param mem_block The memory block to write data to.
 * @param data The data to write.
 * @param size The size of the data. If size is 0, it will auto-calculate based 
 *             on data type (for example, string length).
 * 
 * @return 0 on success, -1 on failure.
 * 
 * Example usage:
 * ```
 * char* my_data = "Hello, World!";
 * if (memory_write(mem_block, my_data, 0) == 0) {
 *     // Successfully written data to memory block
 * }
 * ```
 */
int memory_write(void* mem_block, const void* data, size_t size) {
    if (!mem_block || !data) return -1;

    // If size is 0, automatically determine size based on the data type (e.g., string length)
    if (size == 0) {
        if (strchr((const char*)data, '\0')) {
            size = strlen((const char*)data) + 1;  // Include the null terminator
        } else {
            size = sizeof(data);  // For non-string data
        }
    }

    // Write data to the memory block
    memcpy(mem_block, data, size);
    return 0;
}

/**
 * @brief Reads data from the allocated memory block into the provided buffer.
 * 
 * This function reads the specified amount of data from the memory block and 
 * stores it in the provided buffer.
 * 
 * @param mem_block The memory block to read data from.
 * @param buffer The buffer to store the data.
 * @param size The size of the data to read.
 * 
 * @return 0 on success, -1 on failure.
 * 
 * Example usage:
 * ```
 * char buffer[256];
 * if (memory_read(mem_block, buffer, sizeof(buffer)) == 0) {
 *     // Successfully read data into buffer
 * }
 * ```
 */
int memory_read(const void* mem_block, void* buffer, size_t size) {
    if (!mem_block || !buffer) return -1;
    memcpy(buffer, mem_block, size);  // Read data from the memory block
    return 0;
}

/**
 * @brief Frees the memory block with the given name.
 * 
 * This function frees the memory block with the specified name and updates 
 * the Memchunk's used and free memory statistics. If adjacent free memory 
 * blocks exist, they will be coalesced to form a larger free block.
 * 
 * @param Memchunk The Memchunk to free the memory from.
 * @param block_name The name of the memory block to free.
 * 
 * Example usage:
 * ```
 * memory_free(chunk, "Block1");  // Frees the memory block named "Block1"
 * ```
 */
void memory_free(Memchunk* Memchunk, const char* block_name) {
    if (!Memchunk || !block_name) return;

    Memory* current = Memchunk->memory_pool;
    while (current) {
        if (!current->is_free && strncmp(current->name, block_name, sizeof(current->name)) == 0) {
            current->is_free = 1;  // Mark the block as free
            Memchunk->used_memory -= current->size;
            Memchunk->free_memory += current->size;
            break;
        }
        current = current->next;
    }

    // Coalesce adjacent free memory blocks if possible
    current = Memchunk->memory_pool;
    while (current) {
        if (current->is_free && current->next && current->next->is_free) {
            current->size += sizeof(Memory) + current->next->size;
            current->next = current->next->next;
        }
        current = current->next;
    }
}

/**
 * @brief Deallocates the memory Memchunk.
 * 
 * WARNING: Before calling this function, ensure that all memory blocks allocated
 * from the Memchunk have been freed, otherwise the program may crash due to invalid
 * memory access or memory leaks.
 * 
 * @param Memchunk The Memchunk to deallocate.
 * 
 * Example usage:
 * ```
 * memc_dealloc(chunk);  // Deallocate the Memchunk
 * ```
 */
void memc_dealloc(Memchunk* Memchunk) {
    if (!Memchunk) return;

    // WARNING: Ensure all memory blocks are freed before calling this function.
    Memchunk->used_memory = 0;
    Memchunk->free_memory = Memchunk->total_size;

    // Free all memory blocks in the Memchunk's memory pool
    Memory* current_mem = Memchunk->memory_pool;
    while (current_mem) {
        Memory* next_mem = current_mem->next;
        free(current_mem);
        current_mem = next_mem;
    }

    // Free the Memchunk structure itself
    free(Memchunk);
}

/**
 * @brief Debug function to display the status of multiple Memchunks.
 * 
 * This function prints details of each Memchunk, including its total size, used 
 * memory, free memory, and the memory blocks inside it.
 * 
 * @param num_Memchunks The number of Memchunks to display.
 * 
 * Example usage:
 * ```
 * memc_dbg(2, chunk1, chunk2);  // Display status of chunk1 and chunk2
 * ```
 */
void memc_dbg(int num_Memchunks, ...) {
    va_list args;
    va_start(args, num_Memchunks);
    for (int i = 0; i < num_Memchunks; i++) {
        Memchunk* curr_Memchunk = va_arg(args, Memchunk*);
        if (curr_Memchunk) {
            printf("Memchunk: %s, Total Size: %zu, Used Memory: %zu, Free Memory: %zu, Next: %p\n", 
                   curr_Memchunk->name, curr_Memchunk->total_size, curr_Memchunk->used_memory, 
                   curr_Memchunk->free_memory, (void*)curr_Memchunk->next);
            Memory* curr_mem = curr_Memchunk->memory_pool;
            while (curr_mem) {
                printf("  Memory Block: %s, Size: %zu, Is Free: %d\n", curr_mem->name, curr_mem->size, curr_mem->is_free);
                curr_mem = curr_mem->next;
            }
        } else {
            printf("Memchunk is NULL\n");
        }
    }
    va_end(args);
}

/**
 * @brief Debug function to display details of memory blocks.
 * 
 * This function prints details of the specified memory blocks.
 * 
 * @param num_mem The number of memory blocks to display.
 * 
 * Example usage:
 * ```
 * mem_dbg(1, block1);  // Display status of block1
 * ```
 */
void mem_dbg(int num_mem, ...) {
    va_list args;
    va_start(args, num_mem);
    for (int i = 0; i < num_mem; i++) {
        Memory* curr_mem = va_arg(args, Memory*);
        if (curr_mem) {
            printf("Memory Block: %s, Size: %zu, Is Free: %d\n", curr_mem->name, curr_mem->size, curr_mem->is_free);
        } else {
            printf("Memory Block is NULL\n");
        }
    }
    va_end(args);
}

/**
 * @brief Frees all memory blocks and deallocates all Memchunks in the global list.
 * 
 * This function traverses the global list of Memchunks and frees all memory blocks
 * in each Memchunk. Afterward, it deallocates the Memchunk itself and resets the global list.
 */
void mem_clr() {
    Memchunk* current_chunk = global_memchunk_list;

    while (current_chunk) {
        // Free all memory blocks in the current Memchunk's memory pool
        Memory* current_mem = current_chunk->memory_pool;
        while (current_mem) {
            Memory* next_mem = current_mem->next;
            free(current_mem);  // Free the current memory block
            current_mem = next_mem;
        }

        // After all memory blocks are freed, free the Memchunk structure itself
        Memchunk* next_chunk = current_chunk->next;
        free(current_chunk);  // Free the Memchunk structure

        // Move to the next Memchunk in the list
        current_chunk = next_chunk;
    }

    // Reset the global list after cleanup
    global_memchunk_list = NULL;
}