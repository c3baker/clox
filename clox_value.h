#ifndef clox_value_h
#define clox_value_h

#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL()  ((Value){VAL_NIL, {.numeric = 0}})
#define NUMERIC_VAL(value)  ((Value){VAL_NUMERIC, {.numeric = value}})

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NUMERIC(value)  ((value).type == VAL_NUMERIC)
#define IS_NIL(value) ((value).type == VAL_NIL)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMERIC(value) ((value).as.numeric)


typedef struct{

 VALUE_TYPE type;
 union{
     bool boolean;
     double numeric;
 }as;   
}Value;

typedef enum
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMERIC
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