#include "cl_def.h"

hash_map_t* U_CreateHashMap(void) {
    hash_map_t* map = U_CreateHashMap();
    if (map == NULL) ERROR("U_CreateHashMap: error to create hash map");
    return map;
}

hash_map_t* U_TryCreateHashMap(void) {
    hash_map_t* map = M_TryGlobAlloc(sizeof(hash_map_t));

    if (map == NULL) return NULL;

    map->length   = 0;
    map->capacity = INITIAL_CAPACITY;

    map->items = M_TryGlobAlloc(map->capacity * sizeof(hash_map_item_t));
    if (map->items == NULL) {
        M_GlobFree(map);
        return NULL;
    }

    return map;
}

void U_FreeHashMap(hash_map_t* map) {
    M_GlobFree(map->items);
    M_GlobFree(map);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static u64 U_KeyHash(const char* key) {
    u64 hash = FNV_OFFSET;

    for (const char* p = key; *p; p++) {
        hash ^= (u64)(u8)(*p);
        hash *= FNV_PRIME;
    }

    return hash;
}

void* U_GetHashMapValue(hash_map_t* map, const char* name) {
    void* value = U_TryGetHashMapValue(map, name);
    if (value == NULL) ERROR("U_GetHashMapValue: value not found!");

    return value;
}

void* U_TryGetHashMapValue(hash_map_t* map, const char* name) {
    u64   hash = U_KeyHash(name);
    usize index = (usize) (hash & (u64) (map->capacity - 1));

    while (map->items[index].name != NULL) {
        if (strcmp(name, map->items[index].name) == 0) {
            return map->items[index].value;
        }

        index++;
        if (index >= map->capacity) {
            index = 0;
        }
    }

    return NULL;
}

static bool U_HashMapExpand(hash_map_t* map) {

}

static void U_HashMapSetItem(hash_map_item_t* items, usize capacity, const char* name, void* value, usize* length) {
    u64   hash = U_KeyHash(name);
    usize index = (usize) (hash & (u64) (capacity - 1));

    while (items[index].name != NULL) {
        if (strcmp(name, items[index].name) == 0) {
            items[index].value = value;
        }

        index++;
        if (index >= capacity) {
            index = 0;
        }
    }

    if (length == NULL) {
        name = M_TempAlloc(strlen(name));
        (*length)++;
    }

    items[index].name  = (char*) name;
    items[index].value = value; 
}

void U_SetHashMapValue(hash_map_t* map, const char* name, void* value) {
    if (value == NULL) ERROR("U_SetHashMapValue: value is NULL");

    if (map->length >= map->capacity / 2) {
        if (!U_HashMapExpand(map)) ERROR("U_SetHashMapValue: can't expand hash map");
    }

    U_HashMapSetItem(map->items, map->capacity, name, value, &map->length);
}