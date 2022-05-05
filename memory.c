#include "common.h"


void* reallocate(void* array, size_t old_size, size_t new_size)
{
    void* result = NULL;

    if(new_size == 0)
    {
        free(array);
        
        return NULL;
    }

    result =  realloc(array, new_size);
    if(result == NULL)
    {
        exit(1);
    }

    return result;
}