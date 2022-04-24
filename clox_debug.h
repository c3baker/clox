#ifndef clox_debug_h
#define clox_debug_h

#include "common.h"
#include "clox_vm.h"

#define CLOX_DEBUG_MODE
#define CLOX_TRACING_MODE

void disassemble_chunk(const CHUNK* chunk, const char* name);
int disassemble_instruction(const CHUNK* chunk, int offset);
void print_stack(const VM* vm);
void print_value(Value v);

#endif
