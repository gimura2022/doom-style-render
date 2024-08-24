#include "../cl_def.h"

extern state_t      client_state;
extern vidstate_t   video_state;
extern game_state_t game_state;

cmd_var_t min_player_speed    = { "sv_min_player_speed",    "", 0, MIN_PLAYER_SPEED };
cmd_var_t regular_impulse     = { "sv_regular_impulse",     "", 0, REGULAR_IMPULSE };
cmd_var_t floor_jump_box_size = { "sv_floor_jump_box_size", "", 0, FLOOR_JUMP_BOX_SIZE };
cmd_var_t jump_impulse        = { "sv_jump_impulse",        "", 0, JUMP_IMPULSE };
cmd_var_t player_eye          = { "sv_player_eye",          "", 0, PLAYER_EYE_Z };
cmd_var_t player_height       = { "sv_player_height",       "", 0, PLAYER_HEIGHT };

static void G_MovePlayer(player_t* player) {
    v3* vel = &player->phys_obj.vel;

    phys_obj_t* phys_obj = &player->phys_obj;

    P_UpdateObject(phys_obj);

    const float impulse = (
        fabsf(vel->x) < min_player_speed.floating &&
        fabsf(vel->y) < min_player_speed.floating &&
        fabsf(vel->z) < min_player_speed.floating
    ) ? min_player_speed.floating : regular_impulse.floating;

    const float anglecos = cos(player->camera.angle.x);
    const float anglesin = sin(player->camera.angle.x);

    if (player->forward) P_AddVel(
        phys_obj,
        (v3) {
            (impulse * anglecos),
            (impulse * anglesin),
            0.f
        }
    );

    if (player->back) P_AddVel(
        phys_obj,
        (v3) {
            -(impulse * anglecos),
            -(impulse * anglesin),
            0.f
        }
    );

    if (player->left) P_AddVel(
        phys_obj,
        (v3) {
            -(impulse * anglesin),
             (impulse * anglecos),
            0.f
        }
    );

    if (player->right) P_AddVel(
        phys_obj,
        (v3) {
             (impulse * anglesin),
            -(impulse * anglecos),
            0.f
        }
    );

    if (player->jump && player->phys_obj.floored) P_AddVel(
        phys_obj,
        (v3) {
            0.f, 0.f,
            jump_impulse.floating
        }
    );
}

static void G_FindPlayerSpawn(player_t* player) {
    v3* pos = &player->phys_obj.obj.pos;

    pos->x = 3;
    pos->y = 3;
    pos->z = 10;
}

void G_InitPlayer(player_t* player) {
    player->phys_obj.mass = 1.0f;
    player->phys_obj.height = player_height.floating;

    G_FindPlayerSpawn(player);
}

void G_UpdatePlayer(player_t* player) {
    G_MovePlayer(player);

    player->phys_obj.height = player_height.floating;

    player->camera.obj.pos.z = player->phys_obj.obj.pos.z + player_eye.floating;
    player->camera.obj.pos.x = player->phys_obj.obj.pos.x;
    player->camera.obj.pos.y = player->phys_obj.obj.pos.y;
    G_UpdateObjectSector(&player->camera.obj);
}