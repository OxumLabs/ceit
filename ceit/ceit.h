#ifndef CEIT_H
#define CEIT_H

#include <stddef.h>

// Forward declarations
typedef struct Memory Memory;
typedef struct Memchunk Memchunk;
extern Memchunk* global_memchunk_list;  // Global pointer to the list of Memchunks

/**
 * @brief Structure representing a block of memory allocated from a Page.
 * Contains metadata for block management.
 */
struct Memory {
    char name[32];          ///< Name of the memory block for easy reference.
    size_t size;            ///< Size of the allocated block.
    int is_free;            ///< Indicates if the block is free (1) or allocated (0).
    Memory* next;           ///< Pointer to the next memory block in the list.
    char data[50];          ///< Flexible array member for block data.
};

/**
 * @brief Structure representing a large memory allocation area (Page) from which
 * smaller blocks (Memory) are allocated.
 */
struct Memchunk {
    char name[32];          ///< Name of the page for reference.
    size_t total_size;      ///< Total size of the Page's memory.
    Memory* memory_pool;    ///< Head pointer to linked list of Memory blocks.

    size_t used_memory;     ///< Used memory in bytes.
    size_t free_memory;     ///< Free memory in bytes.
    Memchunk* next;         ///< Pointer to the next page (if chaining pages).
};

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
 */
Memchunk* memc_init(const char* name, size_t total_size);

/**
 * @brief Allocates memory from the Memchunk's memory pool.
 * 
 * This function searches for the best-fit memory block in the pool that is large
 * enough to satisfy the request and allocates it. If the block is larger than 
 * necessary, it is split. The allocated memory block is marked as used, and 
 * the Memchunk's used/free memory statistics are updated.
 * 
 * @param page The Memchunk from which memory is allocated.
 * @param size The size of the memory block to allocate.
 * @param block_name The name of the allocated memory block.
 * 
 * @return A pointer to the allocated memory block, or NULL if allocation fails.
 */
void* memory_alloc(Memchunk* page, size_t size, const char* block_name);

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
 */
int memory_write(void* mem_block, const void* data, size_t size);

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
 */
int memory_read(const void* mem_block, void* buffer, size_t size);

/**
 * @brief Frees the memory block with the given name.
 * 
 * This function frees the memory block with the specified name and updates 
 * the Memchunk's used and free memory statistics. If adjacent free memory 
 * blocks exist, they will be coalesced to form a larger free block.
 * 
 * @param page The Memchunk to free the memory from.
 * @param block_name The name of the memory block to free.
 */
void memory_free(Memchunk* page, const char* block_name);

/**
 * @brief Deallocates the memory Memchunk.
 * 
 * WARNING: Before calling this function, ensure that all memory blocks allocated
 * from the Memchunk have been freed, otherwise the program may crash due to invalid
 * memory access or memory leaks.
 * 
 * @param page The Memchunk to deallocate.
 */
void memc_dealloc(Memchunk* page);

/**
 * @brief Debug function to display the status of multiple Memchunks.
 * 
 * This function prints details of each Memchunk, including its total size, used 
 * memory, free memory, and the memory blocks inside it.
 * 
 * @param num_pages The number of Memchunks to display.
 */
void memc_dbg(int num_pages, ...);

/**
 * @brief Debug function to display details of memory blocks.
 * 
 * This function prints details of the specified memory blocks.
 * 
 * @param num_mem The number of memory blocks to display.
 */
void mem_dbg(int num_mem, ...);

/**
 * @brief Frees all memory blocks and deallocates all Memchunks in the global list.
 * 
 * This function automatically cleans up all memory blocks allocated in the system
 * and deallocates all Memchunks, starting from the global list.
 * 
 * It does not take any arguments and cleans up everything that was allocated.
 */
void mem_clr();

#endif // CEIT_H
