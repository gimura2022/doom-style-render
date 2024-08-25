#ifndef _r_draw_h
#define _r_draw_h

#include "../../utils/u_def.h"

void D_VertLine(int y0, int y1, int x, u32 color);
void D_TexturedVertLine(int y0, int y1, int x, int u, u16 texture);

void D_HorsLine(int x0, int x1, int y, u32 color);

#endif