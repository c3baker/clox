#include "clox_value.h"

void init_value_array(VALUE_ARRAY* array)
{
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void free_value_array(VALUE_ARRAY* value_array)
{
    if(NULL != FREE_ARRAY(Value, value_array))
    {
        exit(1);
    }

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

    value_array->values[++value_array->count] = value;
}
