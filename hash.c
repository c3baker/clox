/*
 * hash.c
 *
 *  Created on: May 12, 2022
 *      Author: charles
 */


#include "clox_hash.h"
#include "clox_memory.h"
#include "clox_value.h"

#define SET_NULL_ENTRY(entry)do{ \
   entry.key = NULL; \
   entry.value = NIL_VAL(); \
   entry.deleted = false; \
}while(0)

#define KEY_MATCH(entry, key)(entry.key == key)
#define HASH_TABLE_OVERLOADED(h_table_ptr) ((((float)h_table_ptr->count) / ((float)h_table_ptr->capacity)) > MAX_LOAD)
#define IS_TOMBSTONE(entry) (entry.deleted)
#define MAKE_TOMBSTONE(entry) do{ \
    entry.value = NIL_VAL(); \
    entry.deleted = true; \
    }while(0)
static ENTRY* find_entry(HASH_TABLE* h_table, OBJ* key);
static void hash_table_allocation(HASH_TABLE* h_table, size_t size);
static void expand_hash_table(HASH_TABLE* h_table);

void init_hash_table(HASH_TABLE* h_table)
{
    int i = 0;

    h_table->capacity = HASH_TABLE_INIT_CAP;
    h_table->count = 0;
    hash_table_allocation(&h_table->table, 0, HASH_TABLE_INIT_CAP);
}

static void expand_hash_table(HASH_TABLE* h_table)
{
    int i = 0;
    size_t old_capacity = h_table->capacity;
    ENTRY* new_table = NULL;
    ENTRY* old_table = h_table->table;

    hash_table_allocation(&new_table, old_capacity, h_table->capacity * 2);
    h_table->table = new_table;

    for(i = 0; i < old_capacity; i++)
    {
        if(!IS_NULL_ENTRY(old_table[i]) && !IS_TOMBSTONE(old_table[i]))
        {
            // Re-insert the entry since its index may have changed
            ENTRY e = old_table[i];
            SET_NULL_ENTRY(new_table[i]);
            table_insert(h_table, e.key, e.value);
        }
    }

    FREE_ARRAY(old_table);

}

static void hash_table_allocation(ENTRY** table, size_t old_size, size_t new_size)
{
    int i = 0;

    *table = GROW_ARRAY(ENTRY, *table, old_size, sizeof(ENTRY) * new_size);

    for(i = 0; i < new_size; i++)
    {
        SET_NULL_ENTRY((*table)[i]);
    }
}

void free_hash_table(HASH_TABLE* h_table)
{
    FREE_ARRAY(h_table->table);
    h_table->table = NULL;
}

static ENTRY* find_entry(HASH_TABLE* h_table, OBJ* key)
{
    HASH_VALUE h_index = key->hash % h_table->capacity;
    HASH_VALUE start_index = h_index;
    ENTRY* first_tombstone = NULL;

    while(!IS_NULL_ENTRY(h_table->table[h_index]))
    {
        // We cycled around the table and couldn't find the entry
        // replace the tombstone
        if((first_tombstone != NULL) && (h_index == start_index))
        {
            return first_tombstone;
        }

        if(KEY_MATCH(h_table->table[h_index], key))
        {
            return &h_table->table[h_index];
        }
        else if(IS_TOMBSTONE(h_table->table[h_index]) && !first_tombstone)
        {
            first_tombstone = &h_table->table[h_index];
        }

        h_index = (h_index + 1) % h_table->capacity;
    }

    return first_tombstone == NULL ? &h_table->table[h_index] : first_tombstone;
}

bool table_get(HASH_TABLE* h_table, const OBJ* key, Value* value)
{
   ENTRY* e = find_entry(h_table, key);
   if(IS_NULL_ENTRY((*e)))
   {
       return false;
   }
   *value = e->value;
   return true;
}


void table_insert(HASH_TABLE* h_table, const OBJ* key, Value value)
{
    HASH_VALUE h_index = 0;
    ENTRY* entry = NULL;

    if(key == NULL)
        return;

    entry = find_entry(h_table, key);

    if(IS_NILL_ENTRY((*entry)))
    {
        // Insert a new key, value pair
        entry->key = key;
        entry->deleted = false;
        h_table->count++;
    }

    entry->value = value; // Replace old value if key is already in table

    if(HASH_TABLE_OVERLOADED(h_table))
    {
      // Need to expand the hash table
      expand_hash_table(h_table);
    }
}


OBJ* delete_entry(HASH_TABLE* h_table, const OBJ* key)
{
    HASH_VALUE h_index = 0;
    ENTRY* entry = NULL;

    if(key == NULL)
        return NULL;

    entry = find_entry(h_table, key);

    if(!IS_NULL_ENTRY((*entry)))
    {
        MAKE_TOMBSTONE((*entry));
    }

    return entry->key;  // So key object can be freed externally since key object was created externally
}

ENTRY* table_find_string_entry(HASH_TABLE* h_table, const char* chars, size_t len, HASH_VALUE hash)
{
    size_t table_size = h_table->capacity;
    size_t index = hash & table_size;
    ENTRY* last_tombstone = NULL;

    for(;;)
    {
        ENTRY e = h_table->table[index];
        if(IS_NULL_ENTRY(e))
        {
            if(IS_TOMBSTONE(e))
            {
                last_tombstone = &e;
            }
            else
            {
               return (last_tombstone == NULL) ? &e : last_tombstone;
            }
        }
        else
        {
            if((AS_STRING(e.value)->len == len) &&
               (e.key->hash == hash) &&
               (0 == memcmp(AS_STRING(e.value)->c_string, chars, len)))
               {
                   return &e;
               }
        }

        index = (index + 1) % table_size;
    }

    return NULL; // Should not be reachable
}



