#include <stdio.h>
#include "common.h"
#include "clox_debug.h"

int simple_instruction(const char* op_name, int offset);


int simple_instruction(const char* op_name, int offset)
{
    printf("%s\n", op_name);
    return offset + 1;
}

void disassemble_chunk(CHUNK* chunk, const char* name)
{
    int offset = 0;

    printf("== %s == \n", name);
    for(offset = 0; offset < chunk->count)
    {
        offset = disassembleInstruction(chunk, offset);

    }
}


int disassemble_instruction(CHUNK* chunk, int offset)
{
    uint8_t instruction = chunk->code[offset];
    

    printf("%04d", offset);
    switch(instruction)
    {
        case OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
        default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1; 
    }

}


