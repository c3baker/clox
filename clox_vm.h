/*
 * clox_vm.h
 *
 *  Created on: Apr 5, 2022
 *      Author: charles
 */

#ifndef CLOX_VM_H_
#define CLOX_VM_H_

#include "common.h"

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
