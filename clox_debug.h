#ifndef clox_debug_h
#define clox_debug_h

#include "clox_chunk.h"

void disassemble_chunk(CHUNK* chunk, const char* name);
int disassemble_instruction(CHUNK* chunk, int offset);


#endif