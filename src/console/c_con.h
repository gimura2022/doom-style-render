#ifndef _c_con_h
#define _c_con_h

#include "utils/u_def.h"

void CON_Init(void);
void CON_Free(void);
void CON_Draw(void);
void CON_ProcessInput(const char* text);
void CON_Exec(void);

void CON_Printf(const char* msg);

#endif