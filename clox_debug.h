#ifndef clox_debug_h
#define clox_debug_h

#include "clox_chunk.h"

void disassemble_chunk(const CHUNK* chunk, const char* name);
int disassemble_instruction(const CHUNK* chunk, int offset);


#endif
