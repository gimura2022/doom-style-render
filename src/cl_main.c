// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// CL_* - client code
//
// --- DESCRIPTION ---
//
// main file of client
//
// -------------------

#include "cl_def.h"

state_t             client_state; // client state
extern game_state_t game_state;   // link game state

usize     event_count = 0; // app event count
SDL_Event events[128];     // app event list

// console command for exit
// only for client
int CMD_ExitCommand(char* args __attribute__((unused))) {
    client_state.quit = true;
    return CE_SUCCESS;
}

// debug interrupt call
int CMD_DebugBreak(char* args __attribute__((unused))) {
    __asm__ __volatile__("int {$}3":);
    return CE_SUCCESS;
}

// command for toggle console
int CMD_ToggleConsole(char* args __attribute__((unused))) {
    // if console opened, close, else closed open
    client_state.console = !client_state.console;
    return CE_SUCCESS;
}

// client initialization
static void CL_Init(int argc, char** argv) {
#   ifdef DEBUG_PRINT_INIT
    printf("CL_Init\n");
#   endif
    
    SYS_Init(argc, argv); // init SYS module
    M_Init(); // init memory manager

    CON_Printf("--- Client init start ---\n");
    CON_Printf("--- Client base init start ---\n");

    srand(time(NULL)); // init random

    KEY_Init(); // init key manager

    CMD_Init(); // init command executor
    CON_Init(); // init console

    CMD_AddCommand("exit",           &CMD_ExitCommand);   // add command for exit
    CMD_AddCommand("debug_break",    &CMD_DebugBreak);    // add command for debug break
    CMD_AddCommand("toggle_console", &CMD_ToggleConsole); // add toggle console command

    V_Init();   // init video
    R_Init();   // render init

    CON_Printf("--- Client base init end ---\n");

    G_Init();   // init game

    CON_Printf("--- Client init end ---\n");
    CON_Printf("Init time %i ms\n", SDL_GetTicks());
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
                client_state.quit = true;
                break;
            
            default: // else other, continue
                break;
        }
    }
}

// main loop
static void CL_MainLoop(void) {
    // init variable for calculate delta time
    u64 now = SDL_GetPerformanceCounter(),
        last = 0;

    // main loop
    while (!client_state.quit) {
        CL_CheckWindowEvents(); // process events

        // if quit in true, break
        if (client_state.quit) {
            break;
        }

        // calculate delta time
        last = now;
        now = SDL_GetPerformanceCounter();

        client_state.delta_time = (double) ((now - last) * 1000 / (double) SDL_GetPerformanceFrequency());

        KEY_Update(); // process keys

        V_Update(); // update video (clear screen buffer)

        const bool pause = client_state.console;

        switch (client_state.state) {
            case GS_LEVEL:
                G_Render();
                if (!pause) G_Update();

                break;

            default:
                break;
        }

        if (pause) SDL_ShowCursor(SDL_ENABLE);
        else       SDL_ShowCursor(SDL_DISABLE); 

        if (client_state.console) {
            CON_Update(); // update console
            CON_Draw();   // drawing console
        }

        if (!client_state.sleepy) { V_Present(); }
    }
}

// free memory and quit processes
static void CL_Free(void) {
#   ifdef DEBUG_PRINT_INIT
    printf("CL_Free\n");
#   endif

    CON_Free(); // free console
    V_Free();   // free video buffer
    R_Free();   // free renderer

    M_Free(); // free memory manager
}

// entry point for game
int main(int argc, char** argv) {
    CL_Init(argc, argv); // init game
    CL_MainLoop();       // main loop
    CL_Free();           // free memory

    return 0; // exit
}