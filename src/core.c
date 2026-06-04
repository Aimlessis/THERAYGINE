#include "engine.h"
#include <stdarg.h>

void engine_init(EngineCtx *ctx, int width, int height, const char *title) {
    memset(ctx, 0, sizeof(EngineCtx));
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(width, height, title);
    SetTargetFPS(60);

    ctx->camera3d.position   = (Vector3){ 6.0f, 5.0f, 6.0f };
    ctx->camera3d.target     = (Vector3){ 0.0f, 0.0f, 0.0f };
    ctx->camera3d.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    ctx->camera3d.fovy       = 60.0f;
    ctx->camera3d.projection = CAMERA_PERSPECTIVE;
    ctx->cam_yaw             = -135.0f;
    ctx->cam_pitch           = -25.0f;
    ctx->cam_speed           = 8.0f;

    ctx->editor.show_toolbar    = true;
    ctx->editor.show_grid       = true;
    ctx->editor.show_inspector  = true;
    ctx->editor.show_gizmos     = true;
    ctx->editor.show_stats      = true;
    ctx->editor.active_tool     = TOOL_SELECT;
    ctx->editor.selected_entity = ENTITY_NULL;

    ctx->viewport_rt    = LoadRenderTexture(width, height);
    ctx->running        = true;
    ctx->entity_next_id = 0;
}

void engine_shutdown(EngineCtx *ctx) {
    for (int i = 0; i < ctx->entity_count; i++) {
        Entity *e = &ctx->entities[i];
        if (e->mask & COMP_MESH)
            UnloadModel(e->mesh.model);
    }
    UnloadRenderTexture(ctx->viewport_rt);
    CloseWindow();
}

bool engine_should_close(EngineCtx *ctx) {
    return WindowShouldClose() || !ctx->running;
}

void engine_begin_frame(EngineCtx *ctx) {
    ctx->delta_time = GetFrameTime();
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    if (ctx->viewport_rt.texture.width != w || ctx->viewport_rt.texture.height != h) {
        UnloadRenderTexture(ctx->viewport_rt);
        ctx->viewport_rt = LoadRenderTexture(w, h);
    }
}

void engine_end_frame(EngineCtx *ctx) { (void)ctx; }

// ── ENTITIES ──────────────────────────────────

EntityID entity_instantiate(EngineCtx *ctx, const char *name) {
    if (ctx->entity_count >= MAX_ENTITIES) return ENTITY_NULL;
    Entity *e = &ctx->entities[ctx->entity_count];
    memset(e, 0, sizeof(Entity));
    e->id     = ctx->entity_next_id++;
    e->active = true;
    strncpy(e->name, name, ENGINE_MAX_NAME_LEN - 1);
    e->transform.position = Vector3Zero();
    e->transform.rotation = Vector3Zero();
    e->transform.scale    = (Vector3){ 1.0f, 1.0f, 1.0f };
    e->mask               = COMP_TRANSFORM;
    ctx->entity_count++;
    return e->id;
}

void entity_destroy(EngineCtx *ctx, EntityID id) {
    for (int i = 0; i < ctx->entity_count; i++) {
        if (ctx->entities[i].id != id) continue;
        Entity *e = &ctx->entities[i];
        if (e->mask & COMP_MESH)
            UnloadModel(e->mesh.model);
        memmove(&ctx->entities[i], &ctx->entities[i+1],
                sizeof(Entity) * (ctx->entity_count - i - 1));
        ctx->entity_count--;
        if (ctx->editor.selected_entity == id)
            ctx->editor.selected_entity = ENTITY_NULL;
        return;
    }
}

Entity *entity_get(EngineCtx *ctx, EntityID id) {
    for (int i = 0; i < ctx->entity_count; i++)
        if (ctx->entities[i].id == id) return &ctx->entities[i];
    return NULL;
}

static Mesh make_mesh(MeshType type) {
    switch (type) {
        case MESH_CUBE: return GenMeshCube(1.0f, 1.0f, 1.0f);
        default:        return GenMeshCube(1.0f, 1.0f, 1.0f);
    }
}

void entity_add_mesh(EngineCtx *ctx, EntityID id, MeshType type, Color tint) {
    Entity *e = entity_get(ctx, id);
    if (!e) return;
    e->mesh.type        = type;
    e->mesh.model       = LoadModelFromMesh(make_mesh(type));
    e->mesh.tint        = tint;
    e->mesh.cast_shadow = true;
    e->mesh.wireframe   = false;
    e->mask |= COMP_MESH;
}
