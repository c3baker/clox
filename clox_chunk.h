#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "clox_value.h"

typedef enum{
    OP_RETURN
}OP_CODE;

typedef struct {
    uint8_t* code;
    size_t capactiy;
    size_t count;
    VALUE_ARRAY constants;
}CHUNK;

void init_chunk(CHUNK* chunk);
void write_chunk(CHUNK* chunk, uint8_t byte);
void free_chunk(CHUNK* chunk);
int add_constant(CHUNK* chunk, Value value);

#endif