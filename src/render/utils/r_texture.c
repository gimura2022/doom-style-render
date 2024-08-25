#include "../../cl_def.h"

void T_AllocTexture(texture_t* tex) {
    tex->data = M_GlobAlloc(tex->size.x * tex->size.y);
}

void T_FreeTexture(texture_t* tex) {
    M_GlobFree(tex->data);
}

void T_ReadTexture(texture_t* tex, const char* name) {
    char  path[64] = { '\0' };
    char* game_dir = SYS_GetGameDir();

    sprintf(path, "%s%s.bmp", game_dir, name);

    M_TempFree(game_dir);

    BMP* bmp = bopen((char*) path);
    tex->size = (v2i) { get_width(bmp), get_height(bmp) };
    T_AllocTexture(tex);

    for (int i = 0; i < tex->size.x; i++) {
        for (int j = 0; j < tex->size.y; j++) {
            u32 r, g, b;

            {
                u8 _r, _g, _b;
                get_pixel_rgb(bmp, i, j, &_r, &_g, &_b);

                r = (u32) _r;
                g = (u32) _g;
                b = (u32) _b;
            }

            u32 color = 0xFF000000;

            b = b << 16;
            g = g << 8;

            color = color | b;
            color = color | g;
            color = color | r;

            tex->data[j * tex->size.x + i] = color;
        }
    }
}

void T_GenDebugTexture(texture_t* tex) {
    const u32 black = 0xFF0000FF;
    const u32 red   = 0xFFFF00FF;

    for (int i = 0; i < tex->size.x * tex->size.y; i++) {
        u32 color = i % 2 == 0 ? black : red;
        tex->data[i] = color;
    }
}