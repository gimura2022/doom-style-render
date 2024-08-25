#include "me_def.h"

extern editor_state_t editor_state;

#define MOVE_CMD(x, y) ({ x = y; return CE_SUCCESS; })
#define P_MOV(x) MOVE_CMD(x, true)
#define M_MOV(x) MOVE_CMD(x, false)

static int CMD_PlusForward(char* args __attribute__((unused)))  { P_MOV(editor_state.forward); }
static int CMD_MinusForward(char* args __attribute__((unused))) { M_MOV(editor_state.forward); }

static int CMD_PlusBack(char* args __attribute__((unused)))  { P_MOV(editor_state.back); }
static int CMD_MinusBack(char* args __attribute__((unused))) { M_MOV(editor_state.back); }

static int CMD_PlusLeft(char* args __attribute__((unused)))  { P_MOV(editor_state.left); }
static int CMD_MinusLeft(char* args __attribute__((unused))) { M_MOV(editor_state.left); }

static int CMD_PlusRight(char* args __attribute__((unused)))  { P_MOV(editor_state.right); }
static int CMD_MinusRight(char* args __attribute__((unused))) { M_MOV(editor_state.right); }

static int CMD_ToggleConsole(char* args __attribute__((unused))) {
    editor_state.console = !editor_state.console;
    return CE_SUCCESS;
}

static int CMD_AddPoint(char* args) {
    if (strcmp(args, "cursor") == 0) {
        
    }

    return CE_SUCCESS;
}

void ME_SetupCommand(void) {
    CMD_AddCommand("+forward", &CMD_PlusForward);
    CMD_AddCommand("-forward", &CMD_MinusForward);
    CMD_AddCommand("+back",    &CMD_PlusBack);
    CMD_AddCommand("-back",    &CMD_MinusBack);
    CMD_AddCommand("+left",    &CMD_PlusLeft);
    CMD_AddCommand("-left",    &CMD_MinusLeft);
    CMD_AddCommand("+right",   &CMD_PlusRight);
    CMD_AddCommand("-right",   &CMD_MinusRight);

    CMD_AddCommand("toggle_console", &CMD_ToggleConsole);
}

void ME_SetupVariables(void) {

}