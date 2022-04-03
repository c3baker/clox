#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"

typedef enum{
    OP_RETURN
}OP_CODE;

typedef struct {
    uint8_t* code;
    size_t capactiy;
    size_t count;
}CHUNK;

void init_chunk(CHUNK* chunk);
void write_chunk(CHUNK* chunk, uint8_t byte);
void free_chunk(CHUNK* chunk);


#endif