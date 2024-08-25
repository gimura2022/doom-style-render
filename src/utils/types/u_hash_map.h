#ifndef _u_hash_map
#define _u_hash_map

#include "utils/u_def.h"

#define INITIAL_CAPACITY 16

typedef struct {
    const char* name;
    void*       value;
} hash_map_item_t;

typedef struct {
    hash_map_item_t* items;
    size_t capacity;
    size_t length;
} hash_map_t;

hash_map_t* U_CreateHashMap(void);
hash_map_t* U_TryCreateHashMap(void);

void U_FreeHashMap(hash_map_t* map);

void* U_GetHashMapValue(hash_map_t* map, const char* name);
void* U_TryGetHashMapValue(hash_map_t* map, const char* name);

void U_SetHashMapValue(hash_map_t* map, const char* name, void* value);

usize U_HashMapLength(hash_map_t* map);

#endif