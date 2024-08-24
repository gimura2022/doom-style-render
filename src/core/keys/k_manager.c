// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// KEY_* - key manager functions
//
// --- DESCRIPTION ---
//
// key manager
//
// -------------------

#include "cl_def.h"

typedef struct {
    char        command[64]; // command for execution if key pressed
    int         code;        // key code
    key_event_t type;        // press type
} bind_t; // bind type

static key_handler_t handlers[128]; // key handlers array
static bind_t        binds[128];    // binds array
static usize handlers_len = 0;      // count of handlers
static usize binds_len    = 0;      // count of binds

// handler for bind command
static void KEY_HandleBinds(int code, key_event_t type) {
    // enumerate binds
    for (usize i = 0; i < binds_len; i++) {
        // if key code and press type equals
        if (binds[i].code == code && binds[i].type == type) {            
            CMD_ExecuteText(binds[i].command); // execute command in bind
        }
    }
}

// bind command
static int CMD_BindCommand(char* args) {
    const char* key_code_str   = strtok(args, " "); // get key code
    const char* key_press_type = strtok(NULL, " "); // get press type
    const char* command        = strtok(NULL, "");  // get command

    // get key code with a char or scan code integer
    int key_code = 0;
    if (sscanf(
        key_code_str,
        "%i", &key_code
    ) == 1) goto done;
    if (sscanf(
        key_code_str,
        "%c", (char*) &key_code
    ) == 1) goto done;
    
    return 2; // if don't read, exit

    done:;

    // get press type  
    key_event_t type;

    if      (strcmp(key_press_type, "DOWN") == 0)    type = KEY_DOWN;    // only key down
    else if (strcmp(key_press_type, "UP") == 0)      type = KEY_UP;      // only key up
    else if (strcmp(key_press_type, "M_DOWN") == 0)  type = MOUSE_DOWN;  // only mouse down 
    else if (strcmp(key_press_type, "M_UP") == 0)    type = MOUSE_UP;    // only mouse up
    else if (strcmp(key_press_type, "M_WHEEL") == 0) type = MOUSE_WHEEL; // only mouse wheel
    else                                          return 2;        // unkown press type

    // add data to bind structure
    bind_t bind = {};
    bind.code = key_code;
    bind.type = type;
    memcpy(bind.command, command, strlen(command));
    
    binds[binds_len++] = bind; // push bind at last

    return CE_SUCCESS;
}

// init key manager
void KEY_Init(void) {
    KEY_AddKeyHandler(     &KEY_HandleBinds); // add handler for bind command
    CMD_AddCommand("bind", &CMD_BindCommand); // add bind command
}

extern usize     event_count; // app event count
extern SDL_Event events[128]; // app event list

// update key manager
void KEY_Update(void) {
    for (usize i = 0; i < event_count; i++) {
        SDL_Event ev = events[i];

        switch (ev.type) {
            // key down event
            case SDL_KEYDOWN: {
                for (usize j = 0; j < handlers_len; j++) {
                    handlers[j](ev.key.keysym.sym, KEY_DOWN); // call handlers with KEY_DOWN
                }

                break;
            }

            // key up event
            case SDL_KEYUP: {
                for (usize j = 0; j < handlers_len; j++) {
                    handlers[j](ev.key.keysym.sym, KEY_UP); // call handlers with KEY_UP
                }

                break;
            }

            // mouse down event
            case SDL_MOUSEBUTTONDOWN: {
                for (usize j = 0; j < handlers_len; j++) {
                    handlers[j](ev.button.button, MOUSE_DOWN); // call handlers with MOUSE_DOWN
                }

                break;
            }

            // mouse up event
            case SDL_MOUSEBUTTONUP: {
                for (usize j = 0; j < handlers_len; j++) {
                    handlers[j](ev.button.button, MOUSE_UP); // call handlers with MOUSE_UP
                }

                break;
            }

            // mouse wheel
            case SDL_MOUSEWHEEL: {
                for (usize j = 0; j < handlers_len; j++) {
                    handlers[j](0, MOUSE_WHEEL); // call handlers with MOUSE_WHEEL
                }

                break;
            }

            default:
                break;
        }
    }
}

// add key handler
void KEY_AddKeyHandler(key_handler_t fn) {
    handlers[handlers_len++] = fn; // push function pointer at last
}