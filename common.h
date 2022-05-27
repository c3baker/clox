#ifndef clox_common_h
#define clox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_EXIT_SOURCE_FILE_ERROR 74
#define ERROR_EXIT_CLOX_USAGE 64
#define NULL_TERMINATOR '\0'
#define MAX_STACK_SIZE 512

typedef enum
{
    OBJ_STRING
}OBJ_TYPE;

typedef struct obj
{
   struct obj* next;
   uint32_t hash;
   OBJ_TYPE type;
}OBJ;

typedef struct string_obj
{
    OBJ obj;
    size_t len;
    char c_string[];
}CLOX_STRING;

typedef enum
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMERIC,
    VAL_OBJ
}VALUE_TYPE;

typedef struct
{
 VALUE_TYPE type;
 union{
     bool boolean;
     double numeric;
     OBJ* obj;
 }as;
}Value;

typedef struct
{
    int capacity;
    int count;
    Value* values;
}VALUE_ARRAY;


typedef struct hash_entry
{
    OBJ* key;
    Value value;
    bool deleted;
}ENTRY;

typedef struct hash_table
{
    size_t count;
    size_t capacity;
    ENTRY* table;
}HASH_TABLE;

typedef struct{
    int start_line; // The line at which the corresponding CHUNK starts at
    int* line_counts; //Number of bytecodes corresponding to the line (start_line + offset)
    int max_lines;
    int current_offset;
}LINE;

typedef struct {
    uint8_t* code;
    size_t capactiy;
    size_t count;
    VALUE_ARRAY constants;
    LINE lines;
}CHUNK;

typedef struct
{
   CHUNK* chunk;
   uint8_t* ip;
   Value* stack_top;
   OBJ* objects;
   HASH_TABLE strings;
   HASH_TABLE globals;
   Value value_stack[MAX_STACK_SIZE];
}VM;


#endif
