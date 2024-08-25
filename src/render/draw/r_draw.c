#include "../../cl_def.h"

extern vidstate_t     video_state;
extern render_state_t render_state;

void D_VertLine(int y0, int y1, int x, u32 color) {
    for (register int y = y0; y <= y1; y++) {
        video_state.pixels[y * SCREEN_WIDTH + x] = color;
    }
}

void D_TexturedVertLine(int y0, int y1, int x, int _u, u16 tex_id) {
    const texture_t* texture = &render_state.textures[tex_id];
    const int u = _u - ((_u / texture->size.x) * texture->size.x);

    for (register int y = y0; y <= y1; y++) {
        const int v = y - ((y / texture->size.y) * texture->size.y);

        video_state.pixels[y * SCREEN_WIDTH + x]
            = texture->data[v * texture->size.x + u];
    }
}

void D_HorsLine(int x0, int x1, int y, u32 color) {
    for (register int x = x0; x <= x1; x++) {
        video_state.pixels[y * SCREEN_WIDTH + x] = color;
    }
}