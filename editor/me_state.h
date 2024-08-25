#ifndef _me_state_h
#define _me_state_h

#include "utils/u_def.h"
#include "game/g_map.h"

typedef struct {
    struct { v2i    arr[256]; usize n; } points;
    struct { wall_t arr[128]; usize n; } walls;
    int grid_res;

    v2 pos;
    v2i pix_pos;

    bool forward;
    bool back;
    bool left;
    bool right;
    bool jump;

    bool console;

    bool quit;
} editor_state_t;

#endif