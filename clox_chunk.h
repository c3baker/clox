#ifndef clox_chunk_h
#define clox_chunk_h

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
    OP_LESS,
    OP_PRINT,
    OP_POP,
    OP_DEFINE_GLOBAL,
    OP_DEFINE_GLOBAL_LONG,
    OP_GET_GLOBAL,
    OP_GET_GLOBAL_LONG,
    OP_SET_GLOBAL,
    OP_SET_GLOBAL_LONG
}OP_CODE;

int add_constant(CHUNK* chunk, Value value);
void init_chunk_lines(LINE* lines);
void free_chunk_lines(LINE* lines);
void write_line(LINE* lines, int line);
void init_chunk(CHUNK* chunk);
void write_chunk(CHUNK* chunk, uint8_t byte, int line);
void free_chunk(CHUNK* chunk);
void write_constant(CHUNK* chunk, Value value, int line);
int get_code_line(CHUNK* chunk, int code_index);
void write_constant_index(CHUNK* chunk, int index, int line);
#endif
