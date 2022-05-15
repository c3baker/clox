/*
 * object.c
 *
 *  Created on: May 5, 2022
 *      Author: charles
 */

#include "clox_object.h"
#include "clox_memory.h"
#include "clox_hash.h"

#define OBJ_AS_STRING(o_ptr) ((CLOX_STRING*)o_ptr)
#define CLOX_STRING_SIZE(len) sizeof(CLOX_STRING) + ((len + 1) *sizeof(char))  // +1 so there is space to add a null terminator
#define INSERT_OBJECT(_vm, obj_ptr)   do{   \
               obj_ptr->next = _vm->objects;\
              _vm->objects = obj_ptr;       \
         }while(0);
#define REMOVE_OBJECT(_vm, obj_ptr) remove_object_from_list(_vm, obj_ptr)

static OBJ* allocate_object(VM* vm, size_t size, OBJ_TYPE type);
static HASH_VALUE hash(const OBJ* key);

static void remove_object_from_list(VM* vm, OBJ* object)
{
     OBJ* temp = vm->objects;  
     if(temp == object) 
     {                   
         vm->objects = object->next; 
         return;                       
     }                              
     while(temp->next != object )
     {
         if(temp->next == NULL) return;
         temp = temp->next;
     }

     temp->next = object->next;
}
static HASH_VALUE hash(const OBJ* key)
{
  uint32_t hash = 2166136261u;

  if(key->type == OBJ_STRING) // Only support string hashes now
  {
      int i = 0;
      CLOX_STRING* str_obj = OBJ_AS_STRING(key);

      for (i = 0; i < str_obj->len; i++) {
        hash ^= (uint8_t)str_obj->c_string[i];
        hash *= 16777619;
      }
  }

  return hash;
}

OBJ* new_string_object(VM* vm, size_t len, char* str_content)
{
    OBJ* str_obj =  allocate_object(vm, CLOX_STRING_SIZE(len), OBJ_STRING);
    CLOX_STRING* clox_string = OBJ_AS_STRING(str_obj);
    ENTRY* str_table_entry = NULL;

    str_obj->hash = hash(str_obj);
    clox_string->len = len;
    str_table_entry = table_find_string_entry(&vm->strings, str_content, len, str_obj->hash);

    if(!IS_NULL_ENTRY((*str_table_entry)))
    {        
        free_object(vm, str_obj);
        return str_table_entry->key; // Use string object from string internment table
    }

    memcpy(clox_string->c_string, str_content, len);
    clox_string->c_string[len] = NULL_TERMINATOR;
    str_table_entry->key = str_obj;
    str_table_entry->value = OBJ_VAL(clox_string);
    return str_obj;
}

static OBJ* allocate_object(VM* vm, size_t size, OBJ_TYPE type)
{
   OBJ* obj = reallocate(NULL, 0, size);
   obj->type = type;
   obj->hash = 0;
   INSERT_OBJECT(vm, obj);
   return obj;
}

void free_object(VM* vm, OBJ* object)
{
    switch(object->type)
    {
        case OBJ_STRING:
            FREE_ARRAY(OBJ_AS_STRING(object)->c_string);
        default:
            break;
    }
    REMOVE_OBJECT(vm, object);
    free(object);
}

bool object_compare(OBJ* a, OBJ* b)
{
    CLOX_STRING* str_a = NULL;
    CLOX_STRING* str_b = NULL;

    if(GET_OBJ_TYPE(a) != GET_OBJ_TYPE(b))
    {
        return false;
    }

    switch(GET_OBJ_TYPE(a))
    {
    case OBJ_STRING:
        str_a = (CLOX_STRING*)a;
        str_b = (CLOX_STRING*)b;

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
        case OBJ_STRING:
            printf("CLOX STR: %s\n", ((CLOX_STRING*)o)->c_string);
            break;
        default:
            return;
    };
}

OBJ* concatenate_strings(CLOX_STRING* str_1, CLOX_STRING* str_2)
{
    reallocate(str_1, CLOX_STRING_SIZE(str_1->len), CLOX_STRING_SIZE(str_1->len + str_2->len)); //Extend String 1 to fit String 2
    memcpy((void*)(&str_1->c_string[str_1->len]), (void*)str_2->c_string, str_2->len + 1); // Copy the NULL Terminator, reallocate() should create enough space
    str_1->len += str_2->len;
    return (OBJ*)str_1;
}

