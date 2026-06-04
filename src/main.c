#include "engine.h"

#define CONSOLE_H 130
#define HIER_W    220

int main(void) {
    EngineCtx ctx;
    engine_init(&ctx, 1280, 720, "Engine v" ENGINE_VERSION);

    // Demo: un cubo
    EntityID cube = entity_instantiate(&ctx, "Cube");
    entity_add_mesh(&ctx, cube, MESH_CUBE, (Color){80, 140, 200, 255});

    while (!engine_should_close(&ctx)) {
        engine_begin_frame(&ctx);
        camera_update_fly(&ctx);

        BeginDrawing();
            ClearBackground((Color){22, 22, 30, 255});
            renderer_draw_scene(&ctx);
            editor_draw(&ctx);
        EndDrawing();

        engine_end_frame(&ctx);
    }

    engine_shutdown(&ctx);
    return 0;
}
