#include "raygui.h"
#include "engine.h"

#define HIER_W    220
#define INSP_W    260
#define TOOLBAR_H  36
#define CONSOLE_H 130
#define ROW_H      24
#define ITEM_PAD    4

static const char *MESH_NAMES[] = { "Cube" };
static const char *TOOL_NAMES[] = { "[Q] Select","[W] Move","[E] Rotate","[R] Scale" };

static void draw_panel_bg(Rectangle r, const char *label) {
    DrawRectangleRec(r, (Color){30, 30, 38, 245});
    DrawRectangleLinesEx(r, 1, (Color){60, 60, 80, 255});
    if (label) {
        DrawRectangle((int)r.x, (int)r.y, (int)r.width, 20, (Color){45, 45, 60, 255});
        DrawText(label, (int)r.x + 6, (int)r.y + 3, 11, (Color){190, 190, 220, 255});
    }
}

static void draw_toolbar(EngineCtx *ctx) {
    int sw = GetScreenWidth();
    DrawRectangle(0, 0, sw, TOOLBAR_H, (Color){28, 28, 36, 255});
    DrawLine(0, TOOLBAR_H, sw, TOOLBAR_H, (Color){60, 60, 80, 255});

    EditorState *ed = &ctx->editor;
    float x = 8;

    for (int i = 0; i < 4; i++) {
        GuiSetState(ed->active_tool == (EditorTool)i ? STATE_PRESSED : STATE_NORMAL);
        if (GuiButton((Rectangle){x, 4, 90, 28}, TOOL_NAMES[i]))
            ed->active_tool = (EditorTool)i;
        GuiSetState(STATE_NORMAL);
        x += 94;
    }

    x += 20;
    GuiCheckBox((Rectangle){x, 8, 16, 16}, "Grid",   &ed->show_grid);   x += 70;
    GuiCheckBox((Rectangle){x, 8, 16, 16}, "Gizmos", &ed->show_gizmos); x += 80;
    GuiCheckBox((Rectangle){x, 8, 16, 16}, "Stats",  &ed->show_stats);

    char buf[32];
    snprintf(buf, sizeof(buf), "FPS: %d", GetFPS());
    DrawText(buf, sw - 80, 11, 12, (Color){120, 220, 120, 255});
}

static float slider_val(Rectangle r, const char *label, float val, float mn, float mx) {
    DrawText(label, (int)r.x, (int)r.y + 3, 10, (Color){160, 160, 200, 255});
    float nv = val;
    GuiSlider((Rectangle){r.x + 50, r.y, r.width - 50, 16}, NULL, NULL, &nv, mn, mx);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.2f", nv);
    GuiTextBox((Rectangle){r.x + r.width - 48, r.y, 46, 16}, buf, 8, false);
    return nv;
}

static void draw_inspector(EngineCtx *ctx) {
    int sh = GetScreenHeight();
    int sw = GetScreenWidth();
    float top    = TOOLBAR_H + 1;
    float avail_h = sh - top - CONSOLE_H - 1;
    float px     = sw - INSP_W;

    draw_panel_bg((Rectangle){px, top, INSP_W, avail_h}, "  INSPECTOR");

    EditorState *ed = &ctx->editor;
    if (ed->selected_entity == ENTITY_NULL) {
        DrawText("No entity selected", (int)px + 10, (int)top + 30, 11, GRAY);
        return;
    }

    Entity *e = entity_get(ctx, ed->selected_entity);
    if (!e) return;

    float cx = px + 6;
    float cy = top + 24;
    float fw = INSP_W - 12;

    GuiTextBox((Rectangle){cx, cy, fw, 22}, e->name, ENGINE_MAX_NAME_LEN - 1, true);
    cy += 26;
    GuiCheckBox((Rectangle){cx, cy, 16, 16}, "Active", &e->active);
    cy += 24;
    DrawLine((int)cx, (int)cy, (int)(cx+fw), (int)cy, (Color){60,60,80,255});
    cy += 6;

    if (e->mask & COMP_TRANSFORM) {
        DrawText("TRANSFORM", (int)cx, (int)cy, 11, (Color){160, 200, 240, 255});
        cy += 16;
        TransformComp *t = &e->transform;
        float rw  = fw - 4;
        float lim = 50.0f;

        DrawText("Pos", (int)cx, (int)cy+3, 10, LIGHTGRAY);
        t->position.x = slider_val((Rectangle){cx+26, cy, rw-26, 16}, "X", t->position.x, -lim, lim); cy += 20;
        t->position.y = slider_val((Rectangle){cx+26, cy, rw-26, 16}, "Y", t->position.y, -lim, lim); cy += 20;
        t->position.z = slider_val((Rectangle){cx+26, cy, rw-26, 16}, "Z", t->position.z, -lim, lim); cy += 24;

        DrawText("Rot", (int)cx, (int)cy+3, 10, LIGHTGRAY);
        t->rotation.x = slider_val((Rectangle){cx+26, cy, rw-26, 16}, "X", t->rotation.x, -180, 180); cy += 20;
        t->rotation.y = slider_val((Rectangle){cx+26, cy, rw-26, 16}, "Y", t->rotation.y, -180, 180); cy += 20;
        t->rotation.z = slider_val((Rectangle){cx+26, cy, rw-26, 16}, "Z", t->rotation.z, -180, 180); cy += 24;

        DrawText("Scl", (int)cx, (int)cy+3, 10, LIGHTGRAY);
        t->scale.x = slider_val((Rectangle){cx+26, cy, rw-26, 16}, "X", t->scale.x, 0.01f, 10); cy += 20;
        t->scale.y = slider_val((Rectangle){cx+26, cy, rw-26, 16}, "Y", t->scale.y, 0.01f, 10); cy += 20;
        t->scale.z = slider_val((Rectangle){cx+26, cy, rw-26, 16}, "Z", t->scale.z, 0.01f, 10); cy += 24;

        DrawLine((int)cx, (int)cy, (int)(cx+fw), (int)cy, (Color){60,60,80,255});
        cy += 6;
    }

    if (e->mask & COMP_MESH) {
        DrawText("MESH", (int)cx, (int)cy, 11, (Color){160, 200, 240, 255});
        cy += 16;
        DrawText("Type:", (int)cx, (int)cy, 10, LIGHTGRAY);
        DrawText(MESH_NAMES[e->mesh.type], (int)cx+50, (int)cy, 10, WHITE);
        cy += 18;
        GuiCheckBox((Rectangle){cx, cy, 14, 14}, "Wireframe",   &e->mesh.wireframe);   cy += 20;
        GuiCheckBox((Rectangle){cx, cy, 14, 14}, "Cast Shadow", &e->mesh.cast_shadow); cy += 20;
        DrawText("Tint:", (int)cx, (int)cy, 10, LIGHTGRAY); cy += 14;
        GuiColorPicker((Rectangle){cx, cy, fw-30, 80}, NULL, &e->mesh.tint);
    }
}

static void draw_stats(EngineCtx *ctx) {
    float x = HIER_W + 10, y = TOOLBAR_H + 8;
    char buf[64];
    DrawRectangle((int)x-4, (int)y-4, 200, 40, (Color){0,0,0,140});
    snprintf(buf, sizeof(buf), "Entities: %d", ctx->entity_count);
    DrawText(buf, (int)x, (int)y, 11, (Color){200,200,255,220});
    snprintf(buf, sizeof(buf), "Delta: %.2fms", ctx->delta_time * 1000.0f);
    DrawText(buf, (int)x, (int)y+14, 11, (Color){200,200,255,220});
}

static void draw_help(EngineCtx *ctx) {
    (void)ctx;
    float x = HIER_W + 10;
    float y = GetScreenHeight() - CONSOLE_H - 110;
    DrawRectangle((int)x-4, (int)y-4, 240, 75, (Color){0,0,0,120});
    DrawText("RMB + WASD : Fly camera",   (int)x, (int)y,    10, (Color){180,180,180,200});
    DrawText("Shift      : Fast move",    (int)x, (int)y+13, 10, (Color){180,180,180,200});
    DrawText("Q / E      : Up / Down",    (int)x, (int)y+26, 10, (Color){180,180,180,200});
    DrawText("Del        : Delete entity",(int)x, (int)y+39, 10, (Color){180,180,180,200});
    DrawText("F1         : Toggle help",  (int)x, (int)y+52, 10, (Color){180,180,180,200});
}

void editor_draw(EngineCtx *ctx) {
    static bool show_help = true;
    if (IsKeyPressed(KEY_F1)) show_help = !show_help;
    if (IsKeyPressed(KEY_DELETE) && ctx->editor.selected_entity != ENTITY_NULL)
        entity_destroy(ctx, ctx->editor.selected_entity);

    draw_toolbar(ctx);
    draw_inspector(ctx);
    if (ctx->editor.show_stats) draw_stats(ctx);
    if (show_help)              draw_help(ctx);
}
