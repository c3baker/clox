/*
 * clox_object.h
 *
 *  Created on: May 5, 2022
 *      Author: charles
 */

#ifndef CLOX_OBJECT_H_
#define CLOX_OBJECT_H_

#include "common.h"
#include "clox_memory.h"

typedef enum{
    OBJ_STRING
}OBJ_TYPE;

#define GET_OBJ_TYPE(o_ptr)  (o_ptr->type)
#define ALLOCATE_STRING_OBJ(len) (CLOX_STRING*)reallocate(NULL, 0, sizeof(CLOX_STRING) + (len*sizeof(char)))

struct obj
{
   OBJ_TYPE type;
   struct obj* next;
};

typedef struct string_obj
{
    OBJ obj;
    size_t len;
    char c_string[];
}CLOX_STRING;


bool object_compare(OBJ* a, OBJ* b);
void print_object(OBJ* o);

#endif /* CLOX_OBJECT_H_ */
