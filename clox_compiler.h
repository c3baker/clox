#ifndef clox_compiler_h
#define clox_compiler_h

#define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION

#include "common.h"

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
}PRECEDENCE;

bool compile(VM* vm, const char* source, CHUNK* chunk);

#endif
