#ifndef clox_compiler_h
#define clox_compiler_h

#define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION

#include "common.h"
#include "clox_scanner.h"
#include "clox_chunk.h"

typedef struct 
{
    SCANNER* scanner;
    PARSER* parser;
    CHUNK* compiling_chunk;
}COMPILER;

typedef enum
{
   PREC_NONE, // Lowest precedence
   PREC_ASSIGN,
   PREC_OR,
   PREC_AND,
   PREC_EQUALITY,
   PREC_COMPARISON,
   PREC_TERM, //+ -
   PREC_FACTOR, //* /
   PREC_UNARY,
   PREC_CALL,
   PREC_PRIMARY // Highest precedence
}PRECENDENCE;

bool compile(const char* source, CHUNK* chunk);



#endif