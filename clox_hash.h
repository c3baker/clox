/*
 * clox_hash.h
 *
 *  Created on: May 12, 2022
 *      Author: charles
 */

#ifndef CLOX_HASH_H_
#define CLOX_HASH_H_

#include "clox_object.h"

#define HASH_TABLE_INIT_CAP 100
#define MAX_LOAD 0.65

typedef struct hash_entry
{
    OBJ* key;
    Value value;
}ENTRY;

typedef struct hash_table
{
    size_t count;
    size_t capacity;
    ENTRY* table;
}HASH_TABLE;

void init_hash_table(HASH_TABLE* h_table, size_t size);
void free_hash_table(HASH_TABLE** h_table);

#endif /* CLOX_HASH_H_ */
