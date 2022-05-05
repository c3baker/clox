#ifndef clox_value_h
#define clox_value_h

#include "common.h"
#include "clox_object.h"


#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL()  ((Value){VAL_NIL, {.numeric = 0}})
#define NUMERIC_VAL(value)  ((Value){VAL_NUMERIC, {.numeric = value}})
#define OBJ_VAL(value) ((Value){VAL_OBJ, {.obj = (OBJ*)value}})

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NUMERIC(value)  ((value).type == VAL_NUMERIC)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_OBJ(value) ((value).type == VAL_OBJ)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMERIC(value) ((value).as.numeric)
#define AS_OBJ(value) ((value).as.obj)

#define IS_STRING(value) ((IS_OBJ(value)) && (AS_OBJ(value)->type == OBJ_STRING))
#define AS_STRING(value) ((CLOX_STRING*)((CLOX_STRING*)AS_OBJ(value)))
#define C_STRING(value)  ((CLOX_STRING*)AS_OBJ(value))->c_string

typedef struct obj OBJ;

typedef struct{

 VALUE_TYPE type;
 union{
     bool boolean;
     double numeric;
     OBJ* obj;
 }as;   
}Value;

typedef enum
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMERIC,
    VAL_OBJ
}VALUE_TYPE;

typedef struct 
{
    int capacity;
    int count;
    Value* values;
}VALUE_ARRAY;

void init_value_array(VALUE_ARRAY* array);
void free_value_array(VALUE_ARRAY* array);
void write_value(VALUE_ARRAY* array, Value value);
void print_value(Value value);
#endif
