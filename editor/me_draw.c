#include "me_def.h"

extern vidstate_t     video_state;
extern editor_state_t editor_state;

cmd_var_t point_size = { "r_point_size", "", 3, 0.f };

static void ME_DrawLine(v2i a, v2i b, u32 color) {
    a.x = clamp(a.x, 1, SCREEN_WIDTH);
    a.y = clamp(a.y, 1, SCREEN_HEIGHT);

    b.x = clamp(b.x, 1, SCREEN_WIDTH);
    b.y = clamp(b.y, 1, SCREEN_HEIGHT);

    int x1 = floor(a.x);
    int x2 = floor(b.x);

    if (x1 > x2) {
        int temp_i = x1;
        x1 = x2;
        x2 = temp_i;

        v2i temp_v = a;
        a = b;
        b = temp_v;
    }

    float d = (b.y - a.y) / clamp(x2 - x1, 1, SCREEN_WIDTH);
    float y = a.y;
    
    for (register int i = x1; i <= x2; i++) {
        int pix_y = floor(y);
        video_state.pixels[pix_y * SCREEN_WIDTH + i] = color;
        y += d;
    }
}

#define GIRD_SIZE 512

static void ME_DrawGrid(void) {
    for (int i = 0; i < GIRD_SIZE; i += editor_state.grid_res) {
        D_VertLine(
            clamp(editor_state.pix_pos.y,             0, SCREEN_HEIGHT),
            clamp(editor_state.pix_pos.y + GIRD_SIZE, 0, SCREEN_HEIGHT),
            clamp(editor_state.pix_pos.x + i,         0, SCREEN_WIDTH),
            0xFFACACAC
        );

        D_HorsLine(
            clamp(editor_state.pix_pos.x,             0, SCREEN_WIDTH),
            clamp(editor_state.pix_pos.x + GIRD_SIZE, 0, SCREEN_WIDTH),
            clamp(editor_state.pix_pos.y + i,         0, SCREEN_HEIGHT),
            0xFFACACAC
        );
    }
}

static void ME_DrawMap(void) {
    for (int i = 0; i < editor_state.walls.n; i++) {
        const wall_t* wall = &editor_state.walls.arr[i];

        ME_DrawLine(
            (v2i) {
                clamp(wall->a.x + editor_state.pix_pos.x, 0, SCREEN_WIDTH),
                clamp(wall->a.y + editor_state.pix_pos.y, 0, SCREEN_HEIGHT),
            },
            (v2i) {
                clamp(wall->b.x + editor_state.pix_pos.x, 0, SCREEN_WIDTH),
                clamp(wall->b.y + editor_state.pix_pos.y, 0, SCREEN_HEIGHT),
            },
            0xFF00FF00
        );
    }

    for (int i = 0; i < editor_state.points.n; i++) {
        const v2i* point = &editor_state.points.arr[i]; 

        for (
             register int j = (point->x - point_size.integer) + editor_state.pos.x;
                          j < (point->x + point_size.integer) + editor_state.pos.x;
                          j++
        ) {
            for (
                register int k = (point->y - point_size.integer) + editor_state.pos.y;
                             k < (point->y + point_size.integer) + editor_state.pos.y;
                             k++
            ) {
                video_state.pixels[k * SCREEN_WIDTH + j] = 0xFF00FF00;
            }
        }
    }
}

void ME_Draw(void) {
    ME_DrawGrid();
    ME_DrawMap();
}