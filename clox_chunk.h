#ifndef clox_chunk_h
#define clox_chunk_h

#include "clox_value.h"
#include "common.h"

#define LINE_INIT 1
#define MAX_SHORT_CONST_INDEX UINT8_MAX

typedef enum{
    OP_RETURN,
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_ADD,
    OP_MULTIPLY,
    OP_DIV,
    OP_SUB,
    OP_NEGATE,
    OP_TRUE,
    OP_FALSE,
    OP_NIL,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS
}OP_CODE;

typedef struct{
    int start_line; // The line at which the corresponding CHUNK starts at
    int* line_counts; //Number of bytecodes corresponding to the line (start_line + offset)
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
void write_constant(CHUNK* chunk, Value value, int line);
int get_code_line(CHUNK* chunk, int code_index);

#endif
