/*
 * object.c
 *
 *  Created on: May 5, 2022
 *      Author: charles
 */

#include "clox_object.h"



bool object_compare(OBJ* a, OBJ* b)
{

    if(GET_OBJ_TYPE(a) != GET_OBJ_TYPE(b))
    {
        return false;
    }

    switch(GET_OBJ_TYPE(a))
    {
    case CLOX_STRING:
        CLOX_STRING* str_a = (CLOX_STRING*)a;
        CLOX_STRING* str_b = (CLOX_STRING*)b;

        if(str_a->len != str_b->len) return false;
        return (0 == memcmp(str_a->c_string, str_b->c_string, str_a->len));
        break;
    default:
        return false;
    };

    return false;
}

void print_object(OBJ* o)
{
    switch(GET_OBJ_TYPE(o))
    {
    case CLOX_STRING:
        printf("CLOX STR: %s", ((CLOX_STRING*)o)->c_string);
        break;
    default:
        return;
    };
}


