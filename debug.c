#include <stdio.h>
#include "common.h"
#include "clox_debug.h"

int simple_instruction(const char* op_name, int offset);
int constant_instruction(const char* name, const CHUNK* chunk, int offset );
int get_line_number(const CHUNK* chunk, int offset);


void print_value(Value v)
{
    printf(" %d ", v);
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
    printf("%g\n", constant);
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

void print_stack(const VM* vm)
{
    Value* ptr = 0;
    printf("\n TOP -> [ ");
    for(ptr = vm->stack_top; ptr != vm->value_stack; ptr--)
    {
        print_value(*ptr);
    }

    print_value(*vm->value_stack);

    printf(" ]\n");
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
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OP_CONSTANT_LONG:
            return constant_long_instruction("OP_CONSTANT_LONG", chunk, offset);
        case OP_ADD:
            return simple_instruction("OP_ADD", offset);
        case OP_SUB:
            return simple_instruction("OP_SUB", offset);
        case OP_DIV:
            return simple_instruction("OP_SUB", offset);
        case OP_MULTIPLY:
            return simple_instruction("OP_MULTIPLY", offset);
        case OP_NEGATE:
            return simple_instruction("OP_NEGATE", offset);
        default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1; 
    }

}


