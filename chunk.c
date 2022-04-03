#include "clox_chunk.h"


void init_chunk(CHUNK* chunk)
{
    chunk->count = 0;
    chunk->capactiy = 0;
    chunk->code = NULL;
    init_value_array(&chunk->constants);
}

void free_chunk(CHUNK* chunk)
{
    free_value_array(&chunk->constants);
    if(NULL == FREE_ARRAY(uint8_t, chunk->code))
    {
        init_chunk(&chunk);
    }
    else
    {
        exit(1); // Error
    }
}

void write_chunk(CHUNK* chunk, uint8_t byte)
{
    if(chunk->count >= chunk->capactiy)
    {
        size_t new_capacity = GROW_ARRAY_SIZE(chunk->capactiy);
        chunk->code = (uint8_t*)GROW_ARRAY(uint8_t, chunk->code, chunk->capactiy, new_capacity);
        chunk->capactiy = new_capacity;
    }

    chunk->code[++chunk->count] = byte;
}


int add_constant(CHUNK* chunk, Value value)
{
    write_value(&chunk->constants, value);
    return chunk->count - 1;
}