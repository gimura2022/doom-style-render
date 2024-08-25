#include "../cl_def.h"

vidstate_t video_state;
u32*       g_pVidbuf;

void V_Init(void) {
    CON_Printf("Client video init\n");

    ASSERT(
        !SDL_Init(SDL_INIT_VIDEO),
        "V_Init: SDL init error: %s",
        SDL_GetError()
    );

    video_state.window = SDL_CreateWindow(
        "Haltura engine",
        SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        INIT_WINDOW_W,
        INIT_WINDOW_H,
        0
    );

    ASSERT(
        video_state.window != NULL,
        "V_Init: failed to create SDL window: %s",
        SDL_GetError()
    );

    video_state.renderer = SDL_CreateRenderer(
        video_state.window,
        -1,
        SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC
    );

    video_state.texture = SDL_CreateTexture(
        video_state.renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    video_state.debug = SDL_CreateTexture(
        video_state.renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_TARGET,
        128,
        128
    );

    video_state.pixels = M_GlobAlloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    g_pVidbuf = video_state.pixels;
}

void V_Present(void) {
    void* px;
    int pitch;

    SDL_LockTexture(video_state.texture, NULL, &px, &pitch);
    {
        for (usize y = 0; y < SCREEN_HEIGHT; y++) {
            memcpy(
                &((u8*)px)[y * pitch],
                &video_state.pixels[y * SCREEN_WIDTH],
                SCREEN_WIDTH * 4
            );
        }
    }
    SDL_UnlockTexture(video_state.texture);

    SDL_SetRenderTarget(video_state.renderer, NULL);
    SDL_SetRenderDrawColor(video_state.renderer, 0, 0, 0, 0xFF);
    SDL_SetRenderDrawBlendMode(video_state.renderer, SDL_BLENDMODE_NONE);

    SDL_RenderClear(video_state.renderer);
    SDL_RenderCopyEx(
        video_state.renderer,
        video_state.texture,
        NULL,
        NULL,
        0.0,
        NULL,
        SDL_FLIP_VERTICAL
    );

    SDL_SetTextureBlendMode(video_state.debug, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(video_state.renderer, video_state.debug, NULL, &((SDL_Rect) { 0, 0, 512, 512 }));
    SDL_RenderPresent(video_state.renderer);
}

void V_Update(void) {
#   ifdef DEV_BUILD
    memset(video_state.pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
#   endif
}

void V_Free(void) {
    SDL_DestroyTexture(video_state.debug);
    SDL_DestroyTexture(video_state.texture);
    SDL_DestroyRenderer(video_state.renderer);
    SDL_DestroyWindow(video_state.window);

    M_GlobFree(video_state.pixels);
}