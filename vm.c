/*
 * vm.c
 *
 *  Created on: Apr 5, 2022
 *      Author: charles
 */

#include "clox_vm.h"
#include "clox_compiler.h"
#include <stdarg.h>

static INTERPRET_RESULT run(VM* vm);
static Value peek(VM* vm, int distance);
static void runtime_error(VM* vm, const char* format, ...);
static bool is_falsey(Value value);
static bool values_equal(Value a, Value b);

static bool values_equal(Value a, Value b)
{
    if(a.type != b.type)
    {
        /* Allow NIL and 0 to be equal */
        if(IS_NIL(a) && (IS_NUMERIC(b) && AS_NUMERIC(b) == 0)) return true;
        if(IS_NIL(b) && (IS_NUMERIC(a) && AS_NUMERIC(a) == 0)) return true;
        return false;
    }
    
    switch(a.type)
    {
        case VAL_BOOL:
            return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NUMERIC:
            return AS_NUMERIC(a) == AS_NUMERIC(b);
        case VAL_NIL:
            return true; // Both are NIL so they are equal
        case VAL_OBJ:
             return object_compare(AS_OBJ(a), AS_OBJ(b));
        default:
           return false;
    }
}


VM* init_VM(void)
{
    VM* vm = malloc(sizeof(VM));
    if(vm == NULL)
    {
        return NULL;
    }

    vm->stack_top = vm->value_stack;
    vm->ip = NULL;
    vm->objects = NULL;
    
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
    CHUNK chunk = {0};
    INTERPRET_RESULT result = 0;
    init_chunk(&chunk);

    if(!compile(source, &chunk))
    {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm->chunk = &chunk;
    vm->ip = vm->chunk->code;

    result = run(vm);

    free_chunk(&chunk);

    return result;    
}

static void runtime_error(VM* vm, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);


    size_t instruction = vm->ip - vm->chunk->code - 1;
    int line = get_code_line(vm->chunk, instruction);
    fprintf(stderr, "[Line %d] in script\n", line);
    reset_stack();
}


static INTERPRET_RESULT run(VM* vm)
{
#define READ_BYTE(_vm) (*_vm->ip++)
#define READ_CONSTANT(_vm) (_vm->chunk->constants.values[READ_BYTE(_vm)])
#define READ_LONG_CONSTANT(_vm) (_vm->chunk->constants.values[(READ_BYTE(_vm) | READ_BYTE(_vm)<<8 | READ_BYTE(_vm)<<16)])
#define NUMERIC_BINARY_OP(_vm, VALUE_TYPE, op) \
        do{ \
           if(!IS_NUMERIC(peek(_vm, 0)) || !IS_NUMERIC(peek(_vm, 1))) \
           { \
               return INTERPRET_RUNTIME_ERROR; \
           } \
           double b = AS_NUMERIC(pop(_vm)); \
           double a = AS_NUMERIC(pop(_vm)); \
           push(_vm, VALUE_TYPE(a op b)); \
          }while(0)
    while(1)
    {
#ifdef CLOX_DEBUG_MODE
        disassemble_instruction(vm->chunk, vm->ip - vm->chunk->code);
#endif
#ifdef CLOX_TRACING_MODE
        print_stack(vm);
#endif
        uint8_t instruction = READ_BYTE(vm);        
        switch(instruction)
        {
            case OP_RETURN:
                return INTERPRET_OK;
            case OP_CONSTANT:
                push(vm,  READ_CONSTANT(vm));
                return INTERPRET_OK;
            case OP_CONSTANT_LONG:
                push(vm, READ_LONG_CONSTANT(vm));
                return INTERPRET_OK;
            case OP_NEGATE:
                Value v = pop(vm);
                if(!IS_NUMERIC(v))
                {
                    runtime_error("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(vm, NUMERIC_VAL(-AS_NUMERIC(v)));
                return INTERPRET_OK;
            case OP_ADD:
                if(IS_STRING(peek(vm, 0)) && IS_STRING(peek(vm, 1)))
                {
                    string_plus_op(vm);
                }
                else
                {
                    NUMERIC_BINARY_OP(vm, NUMERIC_VAL, +);
                }
                return INTERPRET_OK;
            case OP_MULTIPLY:
                NUMERIC_BINARY_OP(vm, NUMERIC_VAL, *);
                return INTERPRET_OK;
            case OP_DIV:
                NUMERIC_BINARY_OP(vm, NUMERIC_VAL, /);
                return INTERPRET_OK;
            case OP_SUB:
                NUMERIC_BINARY_OP(vm, NUMERIC_VAL, -);
                return INTERPRET_OK;
            case OP_TRUE:
                push(vm, BOOL_VAL(true));
                return INTERPRET_OK;
            case OP_FALSE:
                push(vm, BOOL_VAL(false));
                return INTERPRET_OK;
            case OP_NIL:
                push(vm, NIL_VAL());
                return INTERPRET_OK;     
            case OP_NOT:
                push(vm, BOOL_VAL(is_falsey(pop(vm))));
                return INTERPRET_OK;     
            case OP_EQUAL:
                Value a = pop(vm);
                Value b = pop(vm);
                push(vm, BOOL_VAL(values_equal(a, b)));
                return INTERPRET_OK;     
            case OP_LESS:
                NUMERIC_BINARY_OP(vm, BOOL_VAL, <);
                return INTERPRET_OK;     
            case OP_GREATER:
                NUMERIC_BINARY_OP(vm, BOOL_VAL, >);
                return INTERPRET_OK;
            default:
                printf("UNKNOWN OP CODE\n");
                break;
        }
    }

#undef NUMERIC_BINARY_OP
#undef READ_LONG_CONSTANT
#undef READ_CONSTANT
#undef READ_BYTE
}

static Value peek(VM* vm, int distance)
{
    return vm->stack_top[-1-distance];
}

static bool is_falsey(Value value)
{
    return IS_NIL(value) || (IS_BOOL(value) && AS_BOOL(value) == false);
}

static void string_plus_op(VM* vm)
{
    Value str_1 = pop(vm);
    Value str_2 = pop(vm);

    Value result_str = OBJ_VAL(concatenate_strings(AS_STRING(str_1), AS_STRING(str_2)));
    push(vm, result_str);
}
