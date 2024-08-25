#include "cl_def.h"

game_state_t      game_state;
extern state_t    client_state;
extern vidstate_t video_state;

#define MOVE_CMD(x, y) ({ x = y; return CE_SUCCESS; })
#define P_MOV(x) MOVE_CMD(x, true)
#define M_MOV(x) MOVE_CMD(x, false)

static int CMD_PlusForward(char* args __attribute__((unused)))  { P_MOV(game_state.player.forward); }
static int CMD_MinusForward(char* args __attribute__((unused))) { M_MOV(game_state.player.forward); }

static int CMD_PlusBack(char* args __attribute__((unused)))  { P_MOV(game_state.player.back); }
static int CMD_MinusBack(char* args __attribute__((unused))) { M_MOV(game_state.player.back); }

static int CMD_PlusLeft(char* args __attribute__((unused)))  { P_MOV(game_state.player.left); }
static int CMD_MinusLeft(char* args __attribute__((unused))) { M_MOV(game_state.player.left); }

static int CMD_PlusRight(char* args __attribute__((unused)))  { P_MOV(game_state.player.right); }
static int CMD_MinusRight(char* args __attribute__((unused))) { M_MOV(game_state.player.right); }

static int CMD_PlusJump(char* args __attribute__((unused)))  { P_MOV(game_state.player.jump); }
static int CMD_MinusJump(char* args __attribute__((unused))) { M_MOV(game_state.player.jump); }

static cmd_var_t sens         = { "cl_sens", "", 0, 0.005f };
static cmd_var_t min_vert_ang = { "cl_min_vert_ang", "", 0, -1.0f };
static cmd_var_t max_vert_ang = { "cl_max_vert_ang", "", 0,  1.0f };

static cmd_var_t tick_rate = { "sv_tick_rate", "", TICK_RATE, 0.f };

extern cmd_var_t gravity;
extern cmd_var_t axel;
extern cmd_var_t air_axel;

extern cmd_var_t min_player_speed;
extern cmd_var_t regular_impulse;
extern cmd_var_t floor_jump_box_size;
extern cmd_var_t jump_impulse;
extern cmd_var_t player_eye;
extern cmd_var_t player_height;

static int CMD_TeleportPlayer(char* args) {
    v3* pos = &game_state.player.phys_obj.obj.pos;

    if (sscanf(
        args,
        "%f %f %f",
        &pos->x,
        &pos->y,
        &pos->z
    ) != 3) return 2;

    return CE_SUCCESS;
}

static int CMD_LoadMap(char* args) {
    G_LoadMap(&game_state.map, args);
    return CE_SUCCESS;
}

void G_Init(void) {
    CON_Printf("--- Game init start ---\n");

    CMD_AddCommand("+forward", &CMD_PlusForward);
    CMD_AddCommand("-forward", &CMD_MinusForward);
    CMD_AddCommand("+back",    &CMD_PlusBack);
    CMD_AddCommand("-back",    &CMD_MinusBack);
    CMD_AddCommand("+left",    &CMD_PlusLeft);
    CMD_AddCommand("-left",    &CMD_MinusLeft);
    CMD_AddCommand("+right",   &CMD_PlusRight);
    CMD_AddCommand("-right",   &CMD_MinusRight);
    CMD_AddCommand("+jump",    &CMD_PlusJump);
    CMD_AddCommand("-jump",    &CMD_MinusJump);

    CMD_AddCommand("pl_tp", &CMD_TeleportPlayer);

    CMD_AddCommand("map", &CMD_LoadMap);

    CMD_AddVariable(&sens);
    CMD_AddVariable(&min_vert_ang);
    CMD_AddVariable(&max_vert_ang);

    CMD_AddVariable(&tick_rate);

    CMD_AddVariable(&gravity);
    CMD_AddVariable(&axel);
    CMD_AddVariable(&air_axel);

    CMD_AddVariable(&min_player_speed);
    CMD_AddVariable(&regular_impulse);
    CMD_AddVariable(&floor_jump_box_size);
    CMD_AddVariable(&jump_impulse);
    CMD_AddVariable(&player_eye);
    CMD_AddVariable(&player_height);

    G_InitPlayer(&game_state.player);

    CMD_ExecuteText("exec game.cfg"); // execute game config

    CON_Printf("--- Game init end ---\n");
}

static void G_HandleMouse(void) {
    int width, height;
    SDL_GetWindowSize(video_state.window, &width, &height);

    int x, y;
    SDL_GetMouseState(&x, &y);

    player_t* player = &game_state.player;

    player->camera.angle.x += sens.floating * (width / 2 - x);
    player->camera.angle.y -= sens.floating * (height / 2 - y);

    player->camera.angle.y = clamp(player->camera.angle.y, min_vert_ang.floating, max_vert_ang.floating);

    SDL_WarpMouseInWindow(video_state.window, width / 2, height / 2);
}

static void G_Tick(void) {
    G_UpdatePlayer(&game_state.player);
}

void G_Update(void) {
    G_HandleMouse();
    if ((SDL_GetTicks() % (100 / tick_rate.integer)) == 0) G_Tick();
}

void G_Render(void) {
    R_RenderCameraView(&game_state.player.camera, &game_state.map); // render player view
}

void G_Free(void) {

}