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

#ifndef _c_con_h
#define _c_con_h

#include "cl_def.h"

// TODO: write more comments

#define MAX_LINES 12

void CON_Init(void);
void CON_Free(void);
void CON_Draw(void);
void CON_Update(void);

void CON_Printf(const char* format, ...);

#endif