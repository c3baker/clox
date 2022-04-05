/*
 * vm.c
 *
 *  Created on: Apr 5, 2022
 *      Author: charles
 */

#include "clox_vm.h"

static INTERPRET_RESULT run(VM* vm);

void init_VM(VM* vm)
{

}

void free_VM(VM* vm)
{

}

INTERPRET_RESULT interpret(VM* vm, CHUNK* chunk)
{
    vm->chunk = chunk;
    vm->ip = vm->chunk->code;

    return run(vm);
}

static INTERPRET_RESULT run(VM* vm)
{
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT(index) (vm->chunk->constants.values[(index)])

    while(1)
    {
#ifdef CLOX_DEBUG_MODE
        disassembl_instruction(vm->chunk, vm->ip - vm->chunk->code);
#endif
        uint8_t instruction = READ_BYTE();
        switch(instruction)
        {
            case OP_RETURN:
                return INTERPRET_OK;
            case OP_CONSTANT:
                Value constant = READ_CONSTANT(READ_BYTE());

                return INTERPRET_OK;
            case OP_CONSTANT_LONG:
                uint32_t indx = READ_BYTE();
                Value constant = 0;
                indx |= READ_BYTE()<<8;
                indx |= READ_BYTE()<<16;
                constant = READ_CONSTANT(indx);

                return INTERPRET_OK;

            default:

        }
    }

#undef READ_CONSTANT
#undef READ_BYTE
}

