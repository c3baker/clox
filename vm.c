/*
 * vm.c
 *
 *  Created on: Apr 5, 2022
 *      Author: charles
 */

#include "clox_vm.h"
#include "clox_compiler.h"

static INTERPRET_RESULT run(VM* vm);

VM* init_VM(void)
{
    VM* vm = malloc(sizeof(VM));
    if(vm == NULL)
    {
        return NULL;
    }

    vm->stack_top = vm->value_stack;
    vm->ip = NULL;
    
    return vm;
}

void free_VM(VM** vm)
{
   free(*vm);
   vm = NULL;
}

Value pop(VM* vm)
{
    if(vm->stack_top == vm->value_stack)
    {
        printf("FATAL ERROR: STACK UNDERFLOW!\n");
        exit(1);
    }
    vm->stack_top--;
    return *vm->stack_top;
}

void push(VM* vm, Value v)
{
    if((vm->stack_top - vm->value_stack) == MAX_STACK_SIZE)
    {
        printf("FATAL ERROR: STACK OVERFLOW!\n");
        exit(1);
    }
    *vm->stack_top = v;
    vm->stack_top++;
}

INTERPRET_RESULT interpret(VM* vm, const char* source)
{
    CHUNK chunk;
    init_chunk(&chunk);


    if(!compile(source, &chunk))
    {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm->chunk = &chunk;
    vm->ip = vm->chunk->code;
    
}

static INTERPRET_RESULT run(VM* vm)
{
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
#define READ_LONG_CONSTANT() (vm->chunk->constants.values[(READ_BYTE() | READ_BYTE()<<8 | READ_BYTE()<<16)])
#define BINARY_OP(op) \
        do{ \
           Value b = pop(vm); \
           Value a = pop(vm); \
           push(vm, a op b); \
          }while(0)


    while(1)
    {
#ifdef CLOX_DEBUG_MODE
        disassemble_instruction(vm->chunk, vm->ip - vm->chunk->code);
#endif
#ifdef CLOX_TRACING_MODE
        print_stack(vm);
#endif
        uint8_t instruction = READ_BYTE();
        Value constant = 0;
        switch(instruction)
        {
            case OP_RETURN:
                return INTERPRET_OK;
            case OP_CONSTANT:
                constant = READ_CONSTANT();
                push(vm, constant);
                return INTERPRET_OK;
            case OP_CONSTANT_LONG:
                constant = READ_LONG_CONSTANT();
                push(vm, constant);
                return INTERPRET_OK;
            case OP_NEGATE:
                Value v = pop(vm);
                push(vm, -v);
                return INTERPRET_OK;
            case OP_ADD:
                BINARY_OP(+);
                return INTERPRET_OK;
            case OP_MULTIPLY:
                BINARY_OP(*);
                return INTERPRET_OK;
            case OP_DIV:
                BINARY_OP(/);
                return INTERPRET_OK;
            case OP_SUB:
                BINARY_OP(-);
                return INTERPRET_OK;
            default:
                printf("UNKNOWN OP CODE\n");
                break;
        }
    }

#undef BINARY_OP
#undef READ_LONG_CONSTANT
#undef READ_CONSTANT
#undef READ_BYTE
}

