#ifndef _p_profiler_h
#define _p_profiler_h

#include "cl_def.h"

void P_StartProfileFrame(void);

void P_BeginProfile(const char* name);
void P_EndProfile(const char* name);

#endif