/*
 * hash.c
 *
 *  Created on: May 12, 2022
 *      Author: charles
 */


#include "clox_hash.h"
#include "clox_memory.h"

#define IS_NULL_ENTRY(entry) (entry.key == NULL)
#define KEY_MATCH(entry, key)(entry.key == key)
#define HASH_TABLE_OVERLOADED(h_table_ptr) ((((float)h_table_ptr->count) / ((float)h_table_ptr->capacity)) > MAX_LOAD)

static ENTRY* find_entry(HASH_TABLE* h_table, OBJ* key);

void init_hash_table(HASH_TABLE* h_table, size_t size)
{
    int i = 0;

    h_table->capacity = HASH_TABLE_INIT_CAP;
    h_table->count = 0;
    GROW_ARRAY(ENTRY*, h_table->table, 0, sizeof(ENTRY)*size);

    for(i = 0; i < h_table->capacity; i++)
    {
        h_table->table[i].key = NULL;
        h_table->table[i].value = NIL_VAL();
    }
}

void free_hash_table(HASH_TABLE** h_table)
{
    FREE_ARRAY((*h_table)->table);
    (*h_table)->table = NULL;
    (*h_table) = NULL;
}

static ENTRY* find_entry(HASH_TABLE* h_table, OBJ* key)
{
    HASH_VALUE h_index = key->hash % h_table->capacity;

    while(!IS_NULL_ENTRY(h_table->table[h_index]))
    {
        if(KEY_MATCH(h_table->table[h_index], key))
        {
            return h_table->table[h_index];
        }

        h_index = (h_index + 1) % h_table->capacity;
    }

    return &h_table->table[h_index];
}

void insert_entry(HASH_TABLE* h_table, const OBJ* key, const Value* value)
{
    HASH_VALUE h_index = 0;
    ENTRY* entry = NULL;

    if(key == NULL)
        return;

    entry = find_entry(h_table, key);

    if(IS_NULL_ENTRY(entry))
    {
        // Insert a new key, value pair
        entry->key = key;
        h_table->count++;
    }

    entry->value = value; // Replace old value if key is already in table

    if(HASH_TABLE_OVERLOADED(h_table))
    {
      // Need to allocate a new hash table
    }


}



