#include "memorystore.h"

#define MEM_ALIGNMENT_BYTES 16

// 350mb ->                   350...000
#define UNMANAGED_MEMORY_SIZE 350000000
// 150mb ->                   150...000
#define MANAGED_MEMORY_SIZE   150000000

static uint8_t * unmanaged_memory = NULL;
static uint64_t unmanaged_memory_size = UNMANAGED_MEMORY_SIZE;
static uint8_t * managed_memory = NULL;
static uint64_t managed_memory_size = MANAGED_MEMORY_SIZE;
static uint32_t malloc_mutex_id;

void init_memory_store(void) {
    malloc_mutex_id = platform_init_mutex_and_return_id();
    unmanaged_memory = (uint8_t *)malloc(UNMANAGED_MEMORY_SIZE);
    managed_memory = (uint8_t *)malloc(MANAGED_MEMORY_SIZE);
}

uint8_t * malloc_from_unmanaged(uint64_t size) {
    
    platform_mutex_lock(malloc_mutex_id);
    
    assert(unmanaged_memory != NULL);
    assert(size > 0);
    
    uint32_t padding = 0;
    assert(unmanaged_memory_size >= MEM_ALIGNMENT_BYTES);
    while ((uintptr_t)(void *)unmanaged_memory %
        MEM_ALIGNMENT_BYTES != 0)
    {
        unmanaged_memory += 1;
        padding += 1;
    }
    unmanaged_memory_size -= padding;
    assert(padding < MEM_ALIGNMENT_BYTES);
    assert((uintptr_t)(void *)unmanaged_memory % MEM_ALIGNMENT_BYTES == 0);
    
    uint8_t * return_value = (uint8_t *)(void *)unmanaged_memory;
    assert(unmanaged_memory_size >= size);
    unmanaged_memory += size;
    unmanaged_memory_size -= size;
    
    platform_mutex_unlock(malloc_mutex_id);
    
    return return_value;
};

uint8_t * malloc_from_managed(uint64_t size) {
    
    platform_mutex_lock(malloc_mutex_id);
    
    assert(managed_memory != NULL);
    assert(size > 0);
    
    uint32_t padding = 0; 
    while ((uintptr_t)(void *)managed_memory % MEM_ALIGNMENT_BYTES != 0) {
        managed_memory += 1;
        padding += 1;
    }
    managed_memory_size -= padding;
    assert(padding < MEM_ALIGNMENT_BYTES);
    assert((uintptr_t)(void *)managed_memory % MEM_ALIGNMENT_BYTES == 0);
    
    uint8_t * return_value = managed_memory;
    
    assert(managed_memory_size >= size);
    managed_memory += size;
    managed_memory_size -= size;
    
    platform_mutex_unlock(malloc_mutex_id);
    
    return return_value;
};

void free_from_managed(uint8_t * to_free) {
    (void)to_free;
    
    // TODO: free up memory from the managed store
    return;
};
