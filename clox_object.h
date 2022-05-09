/*
 * clox_object.h
 *
 *  Created on: May 5, 2022
 *      Author: charles
 */

#ifndef CLOX_OBJECT_H_
#define CLOX_OBJECT_H_

#include "common.h"
#include "clox_value.h"
#include "clox_vm.h"

typedef enum{
    OBJ_STRING
}OBJ_TYPE;

struct obj
{
   OBJ_TYPE type;
   struct obj* next;
};

#define GET_OBJ_TYPE(o_ptr)  (o_ptr->type)
#define NEW_STRING(_vm, len, c_string) (CLOX_STRING*)new_string_object(_vm, len, c_string)

typedef struct string_obj
{
    OBJ obj;
    size_t len;
    char c_string[];
}CLOX_STRING;


bool object_compare(OBJ* a, OBJ* b);
void print_object(OBJ* o);
OBJ* concatenate_strings(CLOX_STRING* str_1, CLOX_STRING* str_2);
OBJ* new_string_object(VM* vm, size_t len, char* str_content);
void free_object(OBJ* object);
#endif /* CLOX_OBJECT_H_ */
