#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "clox_value.h"

#define LINE_INIT -1

typedef enum{
    OP_RETURN,
    OP_CONSTANT
}OP_CODE;

typedef struct{
    int start_line;
    int* line_counts;
    int max_lines;
    int current_offset;
}LINE;

typedef struct {
    uint8_t* code;
    size_t capactiy;
    size_t count;
    VALUE_ARRAY constants;
    LINE lines;
}CHUNK;

void init_chunk_lines(LINE* lines);
void free_chunk_lines(LINE* lines);
void write_line(LINE* lines, int line);
void init_chunk(CHUNK* chunk);
void write_chunk(CHUNK* chunk, uint8_t byte, int line);
void free_chunk(CHUNK* chunk);
int add_constant(CHUNK* chunk, Value value);

#endif
