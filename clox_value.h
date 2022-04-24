#ifndef clox_value_h
#define clox_value_h

typedef double Value;

typedef struct 
{
    int capacity;
    int count;
    Value* values;
}VALUE_ARRAY;

void init_value_array(VALUE_ARRAY* array);
void free_value_array(VALUE_ARRAY* array);
void write_value(VALUE_ARRAY* array, Value value);

#endif