
# CEIT (C Enhanced Integrated Toolkit)

CEIT is a superset of the C programming language that introduces custom abstractions for improved safety, ease of use, and efficiency. It builds on the power and simplicity of C while offering additional features to enhance the development experience. In this section, we will focus on the **Memory** system of CEIT, which is designed for managing memory allocations with custom structures and utilities.

## Memory

The memory system in CEIT is based on custom memory chunk management, providing finer control over memory allocation and deallocation. It introduces two key structures:

### Memory Block (Memory)

The `Memory` structure represents a block of memory allocated from a memory chunk. Each `Memory` block contains metadata that helps manage the memory allocation. The structure consists of the following fields:

- **name**: A string representing the name of the memory block (for easy reference).
- **size**: The size of the memory block in bytes.
- **is_free**: A flag indicating whether the memory block is free (`1` for free, `0` for allocated).
- **next**: A pointer to the next memory block in the list (to form a linked list of memory blocks).
- **data**: A flexible array for storing the actual data allocated to the memory block.

### Memory Chunk (Memchunk)

The `Memchunk` structure represents a larger memory region from which smaller memory blocks (`Memory`) are allocated. It acts as a "container" for multiple memory blocks. The fields in `Memchunk` include:

- **name**: A string representing the name of the memory chunk.
- **total_size**: The total size of the memory chunk in bytes.
- **memory_pool**: A pointer to the head of the linked list of `Memory` blocks within the chunk.
- **used_memory**: The total amount of memory used in the chunk (in bytes).
- **free_memory**: The total amount of free memory left in the chunk (in bytes).
- **next**: A pointer to the next `Memchunk` in a chain (for supporting multiple chunks).

### Memory Operations

CEIT offers a set of functions to allocate, manage, and free memory blocks from `Memchunk` structures:

1. **Initialization (`memc_init`)**:
    - This function creates and initializes a new `Memchunk` with the given name and total size. It sets up a linked list of `Memory` blocks, all of which are initially free.

2. **Allocation (`memory_alloc`)**:
    - The `memory_alloc` function searches for the best-fit free memory block in a `Memchunk` and allocates it. If the block is large enough, it can be split into smaller blocks. The allocation process also updates the `Memchunk`'s used and free memory statistics.

3. **Writing Data (`memory_write`)**:
    - Data can be written into a memory block using `memory_write`. If the size is specified as `0`, it will automatically calculate the size based on the type of data, such as string length.

4. **Reading Data (`memory_read`)**:
    - Data can be read from a memory block into a buffer using `memory_read`.

5. **Freeing Memory (`memory_free`)**:
    - When a memory block is no longer needed, it can be freed using `memory_free`. This function marks the block as free and updates the `Memchunk`'s memory usage statistics. Adjacent free blocks are coalesced to form larger free regions, reducing fragmentation.

6. **Deallocation (`memc_dealloc`)**:
    - The `memc_dealloc` function deallocates a `Memchunk`, freeing all memory blocks contained within it. It is important to ensure that all memory blocks within a `Memchunk` are freed before calling this function to avoid memory leaks or invalid memory access.

7. **Cleanup (`mem_clr`)**:
    - `mem_clr` is a utility that automatically cleans up all memory blocks and deallocates all `Memchunk` structures from the global memory chunk list.

This custom memory management system in CEIT provides developers with more granular control over memory operations, making it suitable for applications where memory allocation needs to be tightly controlled.
---
> #####  More fine utilities are in development and we are aiming to turn this into a whole ***superset of C programming language*** with a lot of useful features like tools to debug , analyse , possibly fix and warn users about there code. along with this more utilities will be added to the library itself like printing , better input , better conditions possibly and so on. SO stay tuned and watch this space :3
---
---
this project is under the GLP V3 License and can be found [here](LICENSE)