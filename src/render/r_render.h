#ifndef _r_render_h
#define _r_render_h

#include "../utils/u_def.h"
#include "utils/r_texture.h"
#include "game/g_player.h"
#include "game/g_map.h"
#include "r_camera.h"

#define EYE_Z 1.0f
#define HFOV DEG2RAD(120.0f)
#define VFOV 0.5f

#define ZNEAR 0.0001f
#define ZFAR  128.0f

typedef struct {
    texture_t textures[256];

    u8  portdraw[256];
    u16 y_lo[SCREEN_WIDTH], y_hi[SCREEN_WIDTH];
} render_state_t;

void R_Init(void);
void R_RenderCameraView(camera_t* camera, map_t* map);
void R_Free(void);

#endif