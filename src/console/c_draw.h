// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// CON_* - console functions
//
// --- DESCRIPTION ---
//
// char drawing
//
// -------------------

#ifndef _c_draw_h
#define _c_draw_h

#include "cl_def.h"

#define CHAR_SIZE      12 // char size in char set
#define MAX_LINE_CHARS 32 // max chars in drawing line

void CON_DrawInit(void); // load char set
void CON_DrawFree(void); // free char set texture

void CON_DrawChar(v2i pos, const char c);       // char drawing
void CON_DrawString(v2i pos, const char* text); // string drawing

#endif