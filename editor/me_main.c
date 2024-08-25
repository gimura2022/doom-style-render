#include "me_def.h"

usize     event_count = 0; // app event count
SDL_Event events[128];     // app event list
bool      quit = false;

editor_state_t    editor_state;

static void ME_Init(int argc, char** argv) {
    SYS_Init(argc, argv);
    M_Init();

    KEY_Init();

    CMD_Init();
    CON_Init();
    
    ME_SetupCommand();
    ME_SetupVariables();

    V_Init();
    R_Init();

    editor_state.grid_res = 30;
    editor_state.walls.arr[editor_state.walls.n++] = (wall_t) {
        (v2i) { 0, 0 },
        (v2i) { 40, 50 },
        0
    };

    CMD_ExecuteText("exec editor.cfg");
}

// process SDL events
static void CL_CheckWindowEvents(void) {
    event_count = 0; // sets event count no zero 
    SDL_Event ev;    // variable for event

    // event check loop
    while (SDL_PollEvent(&ev)) {
        events[event_count++] = ev; // write event to event array

        // check type
        switch (ev.type) {
            case SDL_QUIT: // if window close event, quit
                quit = true;
                break;
            
            default: // else other, continue
                break;
        }
    }
}

#define MOVE_SPEED 0.8f

static void ME_MoveCamera(void) {
    if (editor_state.forward) editor_state.pos.y -= MOVE_SPEED;
    if (editor_state.back)    editor_state.pos.y += MOVE_SPEED;
    if (editor_state.left)    editor_state.pos.x += MOVE_SPEED;
    if (editor_state.right)   editor_state.pos.x -= MOVE_SPEED;

    editor_state.pix_pos.x = (int) editor_state.pos.x;
    editor_state.pix_pos.y = (int) editor_state.pos.y;
}

static void ME_MainLoop(void) {
    // init variable for calculate delta time
    u64 now = SDL_GetPerformanceCounter(),
        last = 0;

    // main loop
    while (!editor_state.quit) {
        CL_CheckWindowEvents(); // process events

        // if quit in true, break
        if (editor_state.quit) {
            break;
        }

        // calculate delta time
        last = now;
        now = SDL_GetPerformanceCounter();

        KEY_Update(); // process keys

        V_Update(); // update video (clear screen buffer)

        if (editor_state.console) {
            CON_Update();
            CON_Draw();
        }

        ME_Draw();
        ME_MoveCamera();

        V_Present();
    }
}

static void ME_Free(void) {
    CON_Free();
    R_Free();
    V_Free();
    
    M_Free();
}

int main(int argc, char* argv[]) {
    ME_Init(argc, argv);
    ME_MainLoop();
    ME_Free();

    return 0;
}