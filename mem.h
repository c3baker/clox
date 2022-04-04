#ifndef clox_mem_h
#define clox_mem_h

#define DEFAULT_SIZE 4;

#define GROW_ARRAY_SIZE(capacity) \
        (capacity) < DEFAULT_SIZE ? DEFAULT_SIZE : (capacity) * 2

#define GROW_ARRAY(type, ptr, old_count, new_count)\
                   (type*)reallocate(ptr, sizeof(type) * (old_count), sizeof(type) * (new_count) )

#define FREE_ARRAY(type, ptr)\
                  (type*)reallocate(ptr, 0, 0)

void* reallocate(void* array, size_t old_size, size_t new_size);

#endif
