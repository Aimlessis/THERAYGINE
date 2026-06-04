#include "engine.h"

static Matrix build_transform(TransformComp *t) {
    Matrix T  = MatrixTranslate(t->position.x, t->position.y, t->position.z);
    Matrix Rx = MatrixRotateX(DEG2RAD * t->rotation.x);
    Matrix Ry = MatrixRotateY(DEG2RAD * t->rotation.y);
    Matrix Rz = MatrixRotateZ(DEG2RAD * t->rotation.z);
    Matrix R  = MatrixMultiply(MatrixMultiply(Rz, Ry), Rx);
    Matrix S  = MatrixScale(t->scale.x, t->scale.y, t->scale.z);
    return MatrixMultiply(MatrixMultiply(S, R), T);
}

void renderer_draw_grid(int slices, float spacing) {
    DrawGrid(slices, spacing);
    float len = (float)(slices / 2) * spacing;
    DrawLine3D((Vector3){-len,0,0}, (Vector3){len,0,0}, RED);
    DrawLine3D((Vector3){0,0,-len}, (Vector3){0,0,len}, BLUE);
    DrawLine3D((Vector3){0,0,0},   (Vector3){0,len,0}, GREEN);
}

void renderer_draw_scene(EngineCtx *ctx) {
    BeginMode3D(ctx->camera3d);

    if (ctx->editor.show_grid)
        renderer_draw_grid(40, 1.0f);

    for (int i = 0; i < ctx->entity_count; i++) {
        Entity *e = &ctx->entities[i];
        if (!e->active) continue;
        if ((e->mask & COMP_MESH) && (e->mask & COMP_TRANSFORM)) {
            e->mesh.model.transform = build_transform(&e->transform);
            DrawModel(e->mesh.model, Vector3Zero(), 1.0f, e->mesh.tint);
        }
    }

    EndMode3D();
}
