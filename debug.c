#include <stdio.h>
#include "common.h"
#include "clox_debug.h"

int simple_instruction(const char* op_name, int offset);
int constant_instruction(const char* name, const CHUNK* chunk, int offset );
int get_line_number(const CHUNK* chunk, int offset);


int get_line_number(const CHUNK* chunk, int offset)
{
    int instruction_counter = 0;
    int i = 0;

    for(i = 0; i < chunk->lines.max_lines; i++)
    {
        instruction_counter += chunk->lines.line_counts[i];
        if(instruction_counter > offset)
        {
            return chunk->lines.start_line + i;
        }

    }

    return chunk->lines.start_line;
}

int simple_instruction(const char* op_name, int offset)
{
    printf("%s\n", op_name);
    return offset + 1;
}

int constant_instruction(const char* name, const CHUNK* chunk, int offset )
{
    uint8_t constant_index = chunk->code[offset + 1];
    Value constant = chunk->constants[constant_index];
    printf("%-16s %4d '", name, constant_index);
    printf("%g\n", constant);
    return offset + 2;
}

void disassemble_chunk(const CHUNK* chunk, const char* name)
{
    int offset = 0;

    printf("== %s == \n", name);
    for(offset = 0; offset < chunk->count;)
    {
        offset = disassembleInstruction(chunk, offset);
    }
}


int disassemble_instruction(const CHUNK* chunk, int offset)
{
    uint8_t instruction = chunk->code[offset];

    printf("%04d", offset);
    printf(" %4d ", get_line_numer(chunk, offset));
    switch(instruction)
    {
        case OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
            break;
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
            break;
        default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1; 
    }

}


