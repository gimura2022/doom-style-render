#include "cl_def.h"

typedef struct {
    const char* name;
    u32         time;
} profile_item_t;

static profile_item_t runs_item[256];
static profile_item_t ends_item[256];
static usize runs_count = 0;
static usize ends_count = 0;
static bool capture = false;

void P_StartProfileFrame(void) {
    runs_count = 0;
    ends_count = 0;

    capture = true;
}

void P_BeginProfile(const char* name) {
    if (!capture) return;

    run
}

void P_EndProfile(const char* name) {
    if (!capture) return;
}