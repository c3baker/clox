/*
 * clox_vm.h
 *
 *  Created on: Apr 5, 2022
 *      Author: charles
 */

#ifndef CLOX_VM_H_
#define CLOX_VM_H_

#include "common.h"
#include "clox_chunk.h"
#include "clox_value.h"
#include "clox_hash.h"

#define MAX_STACK_SIZE 512

typedef struct
{
   CHUNK* chunk;
   uint8_t* ip;
   Value* stack_top;
   OBJ* objects;
   HASH_TABLE strings;
   HASH_TABLE globals;
   Value value_stack[MAX_STACK_SIZE];
}VM;

typedef enum
{
    INTERPRET_OK=0,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
}INTERPRET_RESULT;

VM* init_VM(void);
void free_VM(VM** vm);
INTERPRET_RESULT interpret(VM* vm, const char* source);
Value pop(VM* vm);
void push(VM* vm, Value v);


#endif /* CLOX_VM_H_ */
