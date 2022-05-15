/*
 * clox_hash.h
 *
 *  Created on: May 12, 2022
 *      Author: charles
 */

#ifndef CLOX_HASH_H_
#define CLOX_HASH_H_

#include "clox_object.h"
#include <stdbool.h>

#define HASH_TABLE_INIT_CAP 100
#define MAX_LOAD 0.65
#define IS_NULL_ENTRY(entry) (IS_NIL(entry.value))

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

void init_hash_table(HASH_TABLE* h_table);
void free_hash_table(HASH_TABLE* h_table);
void delete_entry(HASH_TABLE* h_table, const OBJ* key);
void insert_entry(HASH_TABLE* h_table, const OBJ* key, Value value);
ENTRY* table_find_string_entry(HASH_TABLE* h_table, const char* chars, size_t len, HASH_VALUE hash);
#endif /* CLOX_HASH_H_ */
