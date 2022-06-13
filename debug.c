#include <stdio.h>
#include "common.h"
#include "clox_debug.h"
#include "clox_chunk.h"
#include "clox_object.h"
#include "clox_value.h"
#include "clox_hash.h"

int simple_instruction(const char* op_name, int offset);
int constant_instruction(const char* name, const CHUNK* chunk, int offset );
int get_line_number(const CHUNK* chunk, int offset);

static int byte_instruction(const char* name, const CHUNK* chunk, int offset)
{
    uint8_t slot = chunk->code[offset+1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

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

void print_constant(const char* name, const CHUNK* chunk, int constant_index)
{
    Value constant = chunk->constants.values[constant_index];
    printf("%-16s %4d '", name, constant_index);
    switch(constant.type)
    {
        case VAL_NUMERIC:
             printf("%g\n", AS_NUMERIC(constant));  
             break;
        case VAL_OBJ:
             print_object(AS_OBJECT(constant));
             break;
        case VAL_NIL:
             printf("NIL\n");
             break;
        default:
            printf("--Unknown Type--\n");
            break; 
    }
}

int constant_instruction(const char* name, const CHUNK* chunk, int offset )
{
    uint8_t constant_index = chunk->code[offset + 1];
    print_constant(name, chunk, constant_index);
    return offset + 2;
}

int constant_long_instruction(const char* name, const CHUNK* chunk, int offset)
{
    uint32_t constant_index = (chunk->code[offset + 1]) | (chunk->code[offset + 2]<<8) | (chunk->code[offset + 3]<<16) ;
    print_constant(name, chunk, constant_index);
    return offset + 4;
}

void disassemble_chunk(const CHUNK* chunk, const char* name)
{
    int offset = 0;

    printf("== %s == \n", name);
    for(offset = 0; offset < chunk->count;)
    {
        offset = disassemble_instruction(chunk, offset);
    }
}

int binary_instruction(uint8_t binary_instruction, int offset)
{
    switch(binary_instruction)
    {
        case OP_ADD:
            printf(" ADD\n");
            break;
        case OP_SUB:
            printf(" SUB\n");
            break;
        case OP_DIV:
            printf(" DIV\n");
            break;
        case OP_MULTIPLY:
            printf(" MULT\n");
            break;
        default:
            printf(" Unknonw Binary Instr\n");
            break;
    }

    return offset + 1;
}

int disassemble_instruction(const CHUNK* chunk, int offset)
{
    uint8_t instruction = chunk->code[offset];

    printf("%04d", offset);
    printf(" %4d ", get_line_number(chunk, offset));
    switch(instruction)
    {
        case OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OP_CONSTANT_LONG:
            return constant_long_instruction("OP_CONSTANT_LONG", chunk, offset);
        case OP_ADD:
        case OP_SUB:
        case OP_DIV:
        case OP_MULTIPLY:
            return binary_instruction(instruction, offset);
        case OP_TRUE:
            return simple_instruction("OP_TRUE", offset);
        case OP_FALSE:
            return simple_instruction("OP_FALSE", offset);
        case OP_NIL:
            return simple_instruction("OP_NIL", offset);
        case OP_NOT:
            return simple_instruction("OP_NOT", offset);
        case OP_EQUAL:
            return simple_instruction("OP_EQUAL", offset);
        case OP_GREATER:
            return simple_instruction("OP_GREATER", offset);
        case OP_LESS:
            return simple_instruction("OP_LESS", offset);
        case OP_PRINT:
            return simple_instruction("OP_PRINT", offset);
        case OP_POP:
            return simple_instruction("OP_POP", offset);
        case OP_DEFINE_GLOBAL:
            return constant_instruction("OP_DEFINE_GLOBAL", chunk, offset);
        case OP_DEFINE_GLOBAL_LONG:
            return constant_long_instruction("OP_DEFINE_GLOBAL_LONG", chunk, offset);
        case OP_GET_GLOBAL:
            return constant_instruction("OP_GET_GLOBAL", chunk, offset);
        case OP_GET_GLOBAL_LONG:
            return constant_long_instruction("OP_GET_GLOBAL_LONG", chunk, offset);
        case OP_GET_LOCAL:
           return byte_instruction("OP_GET_LOCAL", chunk, offset);
        case OP_SET_LOCAL:
           return byte_instruction("OP_SET_LOCAL", chunk, offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1; 
    }

}


