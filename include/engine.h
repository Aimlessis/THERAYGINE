#ifndef ENGINE_H
#define ENGINE_H

#include "raylib.h"
#include "raymath.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ENTITIES        64
#define ENGINE_MAX_NAME_LEN 32
#define ENGINE_VERSION      "0.1.0"

typedef uint32_t ComponentMask;
#define COMP_NONE      0
#define COMP_TRANSFORM (1 << 0)
#define COMP_MESH      (1 << 1)
#define COMP_CAMERA    (1 << 2)

#define ENTITY_NULL -1
typedef int EntityID;

typedef struct {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
} TransformComp;

typedef enum {
    MESH_CUBE = 0,
    MESH_CUSTOM,
} MeshType;

typedef struct {
    MeshType type;
    Model    model;
    Color    tint;
    bool     wireframe;
    bool     cast_shadow;
} MeshComp;

typedef struct {
    EntityID      id;
    char          name[ENGINE_MAX_NAME_LEN];
    bool          active;
    ComponentMask mask;
    MeshComp      mesh;
    TransformComp transform;
} Entity;

typedef enum {
    TOOL_SELECT = 0,
    TOOL_TRANSLATE,
    TOOL_ROTATE,
    TOOL_SCALE,
} EditorTool;

typedef struct {
    bool       show_toolbar;
    bool       show_grid;
    bool       show_inspector;
    bool       show_gizmos;
    bool       show_stats;
    EditorTool active_tool;
    EntityID   selected_entity;
    float      inspector_scroll;
} EditorState;

typedef struct {
    Entity          entities[MAX_ENTITIES];
    int             entity_count;
    int             entity_next_id;
    Camera3D        camera3d;
    float           cam_yaw;
    float           cam_pitch;
    float           cam_speed;
    EditorState     editor;
    float           delta_time;
    bool            camera_active;
    RenderTexture2D viewport_rt;
    Rectangle       viewport_rect;
    bool            running;
} EngineCtx;

void     engine_init(EngineCtx *ctx, int width, int height, const char *title);
void     engine_shutdown(EngineCtx *ctx);
bool     engine_should_close(EngineCtx *ctx);
void     engine_begin_frame(EngineCtx *ctx);
void     engine_end_frame(EngineCtx *ctx);

EntityID entity_instantiate(EngineCtx *ctx, const char *name);
void     entity_destroy(EngineCtx *ctx, EntityID id);
Entity  *entity_get(EngineCtx *ctx, EntityID id);
void     entity_add_mesh(EngineCtx *ctx, EntityID id, MeshType type, Color tint);

void     renderer_draw_scene(EngineCtx *ctx);
void     renderer_draw_grid(int slices, float spacing);

void     camera_update_fly(EngineCtx *ctx);

void     editor_draw(EngineCtx *ctx);

#endif // ENGINE_H
