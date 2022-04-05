/*
 * clox_vm.h
 *
 *  Created on: Apr 5, 2022
 *      Author: charles
 */

#ifndef CLOX_VM_H_
#define CLOX_VM_H_

#include "common.h"
#include "clox_debug.h"

typedef struct
{
   CHUNK* chunk;
   uint8_t* ip;

}VM;

typedef enum
{
    INTERPRET_OK=0,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
}INTERPRET_RESULT;

void init_VM(VM* vm);
void free_VM(VM* vm);
INTERPRET_RESULT interpret(VM* vm, CHUNK* chunk);




#endif /* CLOX_VM_H_ */
