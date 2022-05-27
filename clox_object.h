/*
 * clox_object.h
 *
 *  Created on: May 5, 2022
 *      Author: charles
 */

#ifndef CLOX_OBJECT_H_
#define CLOX_OBJECT_H_

#include "common.h"
#include "clox_vm.h"

#define GET_OBJ_TYPE(o_ptr)  (o_ptr->type)
#define NEW_STRING(_vm, len, c_string) (CLOX_STRING*)new_string_object(_vm, len, c_string)
#define OBJ_TO_STRING(o_ptr) ((CLOX_STRING*)(o_ptr))->c_string


bool object_compare(OBJ* a, OBJ* b);
void print_object(OBJ* o);
OBJ* concatenate_strings(CLOX_STRING* str_1, CLOX_STRING* str_2);
OBJ* new_string_object(VM* vm, size_t len, const char* str_content);
void free_object(VM* vm , OBJ* object);
#endif /* CLOX_OBJECT_H_ */
