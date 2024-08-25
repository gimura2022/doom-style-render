#include "../cl_def.h"

extern state_t g_cState;

static void G_LoadTextWalls(map_t* map, const char* source);
static void G_LoadTextSectors(map_t* map, const char* source);

static void G_LoadTextGeometry(map_t* map, u8* buf, usize size) {
    u8* s_buf = M_TempAlloc(size);
    memcpy(s_buf, buf, size);

    G_LoadTextWalls(map, (char*) buf);
    G_LoadTextSectors(map, (char*) s_buf);

    M_TempFree(buf);
    M_TempFree(s_buf);
}

static void G_LoadTextWalls(map_t* map, const char* source) {
    bool fail = false;
    conf_array_t array = C_GetArray((char*) source, "line");
    map->walls.n = array.s;

    for (usize i = 0; i < array.s; i++) {
        wall_t* wall = &map->walls.arr[i];
        if (sscanf(
            array.d[i], "%d %d %d %d %d %f %f",
            &wall->a.x,
            &wall->a.y,
            &wall->b.x,
            &wall->b.y,
            &wall->portal,
            &wall->f0,
            &wall->f1
        ) != 7) fail = true;
    }

    for (usize i = 0; i < array.s; i++) M_TempFree((void*) array.d[i]);
    ASSERT(!fail, "G_LoadTextWalls: invalid wall format");
}

static void G_LoadTextSectors(map_t* map, const char* source) {
    bool fail = false;
    conf_array_t array = C_GetArray((char*) source, "sect");
    map->sectors.n = array.s + 1;

    for (usize i = 0; i < array.s; i++) {
        sector_t* sector = &map->sectors.arr[i + 1];
        if (sscanf(
            array.d[i], "%d %zu %zu %f %f %hhu",
            &sector->id,
            &sector->firstwall,
            &sector->nwalls,
            &sector->zfloor,
            &sector->zceil,
            &sector->light
        ) != 6) fail = true;
    }

    for (usize i = 0; i < array.s; i++) M_TempFree((void*) array.d[i]);
    ASSERT(!fail, "G_LoadTextSectors: invalid sector format");
}

// static void G_LoadBinGeometry(map_t* map, const map_header_t header) {
//     ERROR("G_LoadBinGeometry: not implemented");
// }

void G_LoadMap(map_t* map, const char* name) {
    CON_Printf("--- Map loading start ---\n");

    char path[64];
    sprintf(path, "maps/%s.prt", name);
    CON_Printf("Loading map from %s\n", path);

    G_LoadTextGeometry(map, SYS_ReadFile(path), SYS_ReadFileSize(path));

    CON_Printf("--- Map loading end ---\n");
}