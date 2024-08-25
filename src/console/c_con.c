// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// CON_* - console functions
//
// --- DESCRIPTION ---
//
// engine console
//
// -------------------

#include "cl_def.h"

static char  con_buf[1024]; // console output buffer
static char  con_in[32];    // console input buffer
static FILE* con_log;       // logging file

extern vidstate_t   video_state;  // link video state

static cmd_var_t console_black_out     = { "con_blackout",      "",   32, 0.f }; // back out value
static cmd_var_t console_input_prefix  = { "con_input_prefix",  "] ", 0,  0.f }; // console input prefix
static cmd_var_t console_print_errors  = { "con_print_errors",  "0",  0,  0.f }; // print errors?
static cmd_var_t console_store_history = { "con_save_logs",     "1",  0,  0.f }; // save logs?

// init console
void CON_Init(void) {
    CON_DrawInit(); // init char drawing (load charset)

    // add variables
    CMD_AddVariable(&console_black_out);
    CMD_AddVariable(&console_input_prefix);
    CMD_AddVariable(&console_print_errors);
    CMD_AddVariable(&console_store_history);
}

// free console
void CON_Free(void) {
    CON_DrawFree(); // free charset

    if (con_log != NULL) fclose(con_log);
}

// draw console
void CON_Draw(void) {
    GFX_Blackout(console_black_out.integer, (v2i) { 0, 0 }, (v2i) { SCREEN_WIDTH, SCREEN_HEIGHT }); // black screen buffer

    CON_DrawString((v2i) { 5, SCREEN_HEIGHT - 10 }, con_buf); // draw console buffer
    CON_DrawString((v2i) { 5, 10 }, con_in);                  // draw input buffer
}

// print to console
void CON_Printf(const char* msg) {
    strcat(con_buf, msg); // add message to back

    // scrolling
    usize lines = 0;
    for (usize i = 0; i < strlen(con_buf); i++) {
        if (con_buf[i] == '\n') lines++;
    }

    if (lines >= MAX_LINES) {
        strtok(con_buf, "\n");
        const char* new_buf = strtok(NULL, "");
        memcpy(con_buf, new_buf, strlen(new_buf));
        con_buf[strlen(new_buf)] = '\0';
    }

    // saving console history
    if (*console_store_history.string != '0') {
        if (con_log == NULL) {
            char path[64];
            sprintf(path, "%scon.log", SYS_GetGameDir());

            con_log = fopen(path, "w");
            if (con_log == NULL) ERROR("CON_Printf: can't open console history file");
        }

        fprintf(con_log, msg);
    }
}

// process input
static void CON_ProcessInput(const char* text) {
    if (strlen(con_in) >= sizeof(con_in)) return; // if input buffer string size > max length of input buffer, return
    strcat(con_in, text);                         // add new text to input buffer
}

// exec command in input buffer
static void CON_Exec(void) {
    char buf[64];
    sprintf(buf, "%s%s", console_input_prefix.string, con_in);

    CON_Printf(buf);

    // execute command
    int ret = CMD_ExecuteText(con_in);
    if (*console_print_errors.string != '0') {
        switch (ret) {
            case CE_NOT_FOUND:
                CON_Printf("\nCommand not found!");
                break;

            case CE_COMMAND_ERROR:
                CON_Printf("\nCommand error!");
                break;

            case CE_SUCCESS:
            case CE_VARIABLE:
            case CE_COMMENT:
            default:
                break;
        }
    }

    // free console input buffer
    memset(con_in, '\0', sizeof(con_in));

    CON_Printf("\n");
}

extern usize     event_count; // app event count
extern SDL_Event events[128]; // app event list

// update console
void CON_Update(void) {
    for (usize i = 0; i < event_count; i++) {
        SDL_Event ev = events[i]; // get event
        
        switch (ev.type) {
            case SDL_TEXTINPUT: // if text input
                CON_ProcessInput(ev.text.text); // process input
                break;

            case SDL_KEYDOWN: // if key down
                if (ev.key.keysym.sym == SDLK_RETURN) CON_Exec(); // if key ENTER, execute command buffer
                break;
            
            default:
                break;
        }
    }
}