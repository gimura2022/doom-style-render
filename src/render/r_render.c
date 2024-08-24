#include "../cl_def.h"

render_state_t render_state;

cmd_var_t z_far     = { "r_zfar",  "", 0, ZFAR };
cmd_var_t z_near    = { "r_znear", "", 0, ZNEAR };
cmd_var_t h_fov     = { "r_hfov",  "", 0, HFOV };
cmd_var_t v_fov     = { "r_vfov",  "", 0, VFOV };
cmd_var_t fulbright = { "r_fulbright", "0", 0, 0.f };

void R_Init(void) {
    CMD_AddVariable(&z_far);
    CMD_AddVariable(&z_near);
    CMD_AddVariable(&h_fov);
    CMD_AddVariable(&v_fov);
    CMD_AddVariable(&fulbright);
    
    texture_t* tex = &render_state.textures[0];
    tex->size = (v2i) { 32, 32 };
    T_AllocTexture(tex);
    T_GenDebugTexture(tex);
}

void R_Free(void) {
    T_FreeTexture(&render_state.textures[0]);
}

#define QUEUE_MAX 64

struct queue_entry { int id, x0, x1; };
struct queue_array { struct queue_entry arr[QUEUE_MAX]; usize n; };

typedef struct {
    const v2 zdl, zdr, znl, znr, zfl, zfr;
    const sector_t* sector;
    struct queue_entry  entry;
    struct queue_array* queue;
} enumerator_data_t;

typedef struct {
    bool has_exit;

    int x, x0, x1, wallshade, tx0, txd, yfd, ycd, yf0, yc0, nyfd, nycd, nyf0, nyc0;
    f32 vert_angl;
} drawer_data_t;

typedef void (*render_drawer_t)     (camera_t*, map_t*, const sector_t*, const wall_t*, drawer_data_t*);
typedef void (*render_enumerator_t) (const enumerator_data_t*, render_drawer_t, camera_t*, map_t*);

static void R_BaseRender(
        render_enumerator_t enumerator, render_drawer_t drawer,
        camera_t* camera, map_t* map
    ) {
    const v2
        zdl = MATH_Rotate(((v2) { 0.0f, 1.0f }), +(h_fov.floating / 2.0f)),
        zdr = MATH_Rotate(((v2) { 0.0f, 1.0f }), -(h_fov.floating / 2.0f)),
        znl = (v2){ zdl.x * z_near.floating, zdl.y * z_near.floating },
        znr = (v2){ zdr.x * z_near.floating, zdr.y * z_near.floating },
        zfl = (v2){ zdl.x * z_far.floating, zdl.y * z_far.floating },
        zfr = (v2){ zdr.x * z_far.floating, zdr.y * z_far.floating };

    struct queue_array queue = {
        {{ camera->obj.sector, 0, SCREEN_WIDTH - 1 }},
        1
    };

    while (queue.n != 0) {
        struct queue_entry entry = queue.arr[--queue.n];
        const sector_t* sector = &map->sectors.arr[entry.id];
        const enumerator_data_t enumerator_data = {
            zdl, zdr, znl, znr, zfl, zfr,
            sector,
            entry,
            &queue
        };

        enumerator(&enumerator_data, drawer, camera, map);
    }
}

static drawer_data_t R_CalculateEnumerateConstants(
        camera_t* camera, map_t* map __attribute__((unused)), const sector_t* sector __attribute__((unused)),
        const enumerator_data_t* enumerator_data,
        v2 point0, v2 point1,
        f32 nz_floor, f32 nz_ceil
    ) {
    drawer_data_t data = {};
    
    // translate relative to player and rotate points around player's view
    const v2 cam_pos = (v2) { camera->obj.pos.x, camera->obj.pos.y };
    const float anglesin = sin(camera->angle.x);
    const float anglecos = cos(camera->angle.x);

    const v2
        op0 = MATH_WorldPosToCamera(point0, cam_pos, anglesin, anglecos),
        op1 = MATH_WorldPosToCamera(point1, cam_pos, anglesin, anglecos);

    // wall clipped pos
    v2 cp0 = op0, cp1 = op1;

    // both are negative -> wall is entirely behind player
    if (cp0.y <= 0 && cp1.y <= 0) {
        goto fail;
    }

    // angle-clip against view frustum
    f32
        ap0 = MATH_NormalizeAngle(atan2(cp0.y, cp0.x) - PI_2),
        ap1 = MATH_NormalizeAngle(atan2(cp1.y, cp1.x) - PI_2);

    // clip against view frustum if both angles are not clearly within
    // HFOV
    if (cp0.y < z_near.floating
        || cp1.y < z_near.floating
        || ap0 > +(h_fov.floating / 2)
        || ap1 < -(h_fov.floating / 2)) {
        const v2
            il = MATH_IntersectSegs(cp0, cp1, enumerator_data->znl, enumerator_data->zfl),
            ir = MATH_IntersectSegs(cp0, cp1, enumerator_data->znr, enumerator_data->zfr);

        // recompute angles if points change
        if (!isnan(il.x)) {
            cp0 = il;
            ap0 = MATH_NormalizeAngle(atan2(cp0.y, cp0.x) - PI_2);
        }

        if (!isnan(ir.x)) {
            cp1 = ir;
            ap1 = MATH_NormalizeAngle(atan2(cp1.y, cp1.x) - PI_2);
        }
    }

    if (ap0 < ap1) {
        goto fail;
    }

    if ((ap0 < -(h_fov.floating / 2) && ap1 < -(h_fov.floating / 2))
        || (ap0 > +(h_fov.floating / 2) && ap1 > +(h_fov.floating / 2))) {
        goto fail;
    }

    // "true" xs before portal clamping
    const int
        tx0 = MATH_ScreenAngleToX(ap0),
        tx1 = MATH_ScreenAngleToX(ap1);

    // bounds check against portal window
    if (tx0 > enumerator_data->entry.x1) { goto fail; }
    if (tx1 < enumerator_data->entry.x0) { goto fail; }

    const int wallshade =
        16 * (sin(atan2f(
            point1.x - point0.x,
            point1.y - point1.y)) + 1.0f);

    const int
        x0 = clamp(tx0, enumerator_data->entry.x0, enumerator_data->entry.x1),
        x1 = clamp(tx1, enumerator_data->entry.x0, enumerator_data->entry.x1);

    const f32
        z_floor  = sector->zfloor,
        z_ceil   = sector->zceil;

    const f32
        sy0 = ifnan((v_fov.floating * SCREEN_HEIGHT) / cp0.y, 1e10),
        sy1 = ifnan((v_fov.floating * SCREEN_HEIGHT) / cp1.y, 1e10);

    const f32 eye_z = camera->obj.pos.z;
    const f32 vert_angl = camera->angle.y * SCREEN_HEIGHT;

    const int
        yf0 = (SCREEN_HEIGHT / 2) + (int)(((z_floor) - eye_z) * sy0),
        yc0 = (SCREEN_HEIGHT / 2) + (int)((z_ceil - eye_z) * sy0),
        yf1 = (SCREEN_HEIGHT / 2) + (int)(((z_floor) - eye_z) * sy1),
        yc1 = (SCREEN_HEIGHT / 2) + (int)((z_ceil - eye_z) * sy1),
        nyf0 = (SCREEN_HEIGHT / 2) + (int)((nz_floor - eye_z) * sy0),
        nyc0 = (SCREEN_HEIGHT / 2) + (int)((nz_ceil - eye_z) * sy0),
        nyf1 = (SCREEN_HEIGHT / 2) + (int)((nz_floor - eye_z) * sy1),
        nyc1 = (SCREEN_HEIGHT / 2) + (int)((nz_ceil - eye_z) * sy1),
        txd = tx1 - tx0 ,
        yfd = yf1 - yf0,
        ycd = yc1 - yc0,
        nyfd = nyf1 - nyf0,
        nycd = nyc1 - nyc0;

    data.x0 = x0;
    data.x1 = x1;
    data.wallshade = wallshade;
    data.tx0 = tx0;
    data.txd = txd;
    data.yfd = yfd;
    data.ycd = ycd;
    data.yf0 = yf0;
    data.yc0 = yc0;
    data.nyfd = nyfd;
    data.nycd = nycd;
    data.nyf0 = nyf0;
    data.nyc0 = nyc0;
    data.vert_angl = vert_angl;

    goto done;

    fail: data.has_exit = true;
    done: return data;
}

static void R_WallEnumerator(
        const enumerator_data_t* enumerator_data, render_drawer_t drawer,
        camera_t* camera, map_t* map
    ) {
    for (usize i = 0; i < enumerator_data->sector->nwalls; i++) {
        const wall_t* wall = &map->walls.arr[enumerator_data->sector->firstwall + i];

        if (render_state.portdraw[enumerator_data->sector->firstwall + i] == 3) continue;
        render_state.portdraw[enumerator_data->sector->firstwall + i]++;                     

        drawer_data_t drawer_data = R_CalculateEnumerateConstants(
            camera, map, enumerator_data->sector,
            enumerator_data,
            v2i_to_v2(wall->a),
            v2i_to_v2(wall->b),
            wall->portal ? map->sectors.arr[wall->portal].zfloor : 0,
            wall->portal ? map->sectors.arr[wall->portal].zceil : 0
        );

        if (drawer_data.has_exit) continue;

        for (int x = drawer_data.x0; x <= drawer_data.x1; x++) {
            drawer_data.x = x;
            drawer(camera, map, enumerator_data->sector, wall, &drawer_data);
        }

        if (wall->portal) {
            ASSERT(enumerator_data->queue->n != QUEUE_MAX, "R_Render: out of queue space");

            enumerator_data->queue->arr[enumerator_data->queue->n++] = (struct queue_entry){
                .id = wall->portal,
                .x0 = drawer_data.x0,
                .x1 = drawer_data.x1
            };
        }
    }
}

static void R_WallRenderer(
        camera_t* camera __attribute__((unused)), map_t* map __attribute__((unused)),
        const sector_t* sector, const wall_t* wall,
        drawer_data_t* data
    ) {
    int shade = data->x == data->x0 || data->x == data->x1 ? 192 : (255 - data->wallshade);
    const u8 light = *fulbright.string == '0' ? sector->light : 255;

    // calculate progress along x-axis via tx{0,1} so that walls
    // which are partially cut off due to portal edges still have
    // proper heights
    const f32 xp = ifnan((data->x - data->tx0) / (f32) data->txd, 0);
    // const f32 u_a = (((1.0f - xp) * z_floor) + (xp * z_ceil))
    //     / (((1.0f - xp) * znl) + (xp * znr));

    // get y coordinates for this x
    const int
        tyf = (int)(xp * data->yfd) + data->yf0,
        tyc = (int)(xp * data->ycd) + data->yc0,
        yf = clamp(tyf + data->vert_angl, render_state.y_lo[data->x], render_state.y_hi[data->x]),
        yc = clamp(tyc + data->vert_angl, render_state.y_lo[data->x], render_state.y_hi[data->x]);

    // floor
    if (yf > render_state.y_lo[data->x]) {
        D_VertLine(
            render_state.y_lo[data->x],
            yf,
            data->x,
            MATH_AbgrMul(0xFFFF0000, light));
    }

    // ceiling
    if (yc < render_state.y_hi[data->x]) {
        D_VertLine(
            yc,
            render_state.y_hi[data->x],
            data->x,
            MATH_AbgrMul(0xFF00FFFF, light));
    }

    if (wall->portal) {
        const int
            tnyf = (int)(xp * data->nyfd) + data->nyf0,
            tnyc = (int)(xp * data->nycd) + data->nyc0,
            nyf = clamp(tnyf + data->vert_angl, render_state.y_lo[data->x], render_state.y_hi[data->x]),
            nyc = clamp(tnyc + data->vert_angl, render_state.y_lo[data->x], render_state.y_hi[data->x]);

        D_VertLine(
            nyc,
            yc,
            data->x,
            MATH_AbgrMul(MATH_AbgrMul(0xFF00FF00, shade), light));

        D_VertLine(
            yf,
            nyf,
            data->x,
            MATH_AbgrMul(MATH_AbgrMul(0xFF0000FF, shade), light));

        render_state.y_hi[data->x] = clamp(min(min(yc, nyc), render_state.y_hi[data->x]), 0, SCREEN_HEIGHT - 1);
        render_state.y_lo[data->x] = clamp(max(max(yf, nyf), render_state.y_lo[data->x]), 0, SCREEN_HEIGHT - 1);
    }
    else {
        D_VertLine(
            yf,
            yc,
            data->x,
            MATH_AbgrMul(MATH_AbgrMul(0xFFFFFFFF, shade), light));
    }
}

void R_RenderCameraView(camera_t* camera, map_t* map) {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        render_state.y_hi[i] = SCREEN_HEIGHT - 1;
        render_state.y_lo[i] = 0;
    }

    memset(render_state.portdraw, 0, sizeof(render_state.portdraw));

    R_BaseRender(&R_WallEnumerator, &R_WallRenderer, camera, map);
}