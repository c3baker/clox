#include "clox_object.h"
#include "clox_value.h"
#include "clox_memory.h"

void init_value_array(VALUE_ARRAY* array)
{
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void free_value_array(VALUE_ARRAY* value_array)
{
    FREE_ARRAY(value_array->values);
    init_value_array(value_array);
}

void write_value(VALUE_ARRAY* value_array, Value value)
{
    if(value_array->count >= value_array->capacity)
    {
        size_t new_capacity = GROW_ARRAY_SIZE(value_array->count);
        value_array->values = GROW_ARRAY(Value, value_array->values, value_array->capacity, new_capacity);
        value_array->capacity = new_capacity;
    }

    value_array->values[value_array->count++] = value;
}

void print_value(Value value)
{
    switch(value.type)
    {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_NIL:
            printf("nil");
            break;
        case VAL_NUMERIC:
            printf("%g", AS_NUMERIC(value));
            break;
        case VAL_OBJ:
            print_object(AS_OBJECT(value));
            break;
    };
}
