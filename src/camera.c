#include "engine.h"
#include <math.h>

void camera_update_fly(EngineCtx *ctx) {
    if (!IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        ctx->camera_active = false;
        ShowCursor();
        return;
    }

    ctx->camera_active = true;
    HideCursor();

    float dt   = ctx->delta_time;
    float sens = 0.15f;

    Vector2 delta  = GetMouseDelta();
    ctx->cam_yaw   += delta.x * sens;
    ctx->cam_pitch -= delta.y * sens;

    if (ctx->cam_pitch >  89.0f) ctx->cam_pitch =  89.0f;
    if (ctx->cam_pitch < -89.0f) ctx->cam_pitch = -89.0f;

    float yawR   = DEG2RAD * ctx->cam_yaw;
    float pitchR = DEG2RAD * ctx->cam_pitch;

    Vector3 forward = {
        cosf(pitchR) * cosf(yawR),
        sinf(pitchR),
        cosf(pitchR) * sinf(yawR)
    };
    forward = Vector3Normalize(forward);

    Vector3 up    = { 0, 1, 0 };
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));

    float speed = ctx->cam_speed;
    if (IsKeyDown(KEY_LEFT_SHIFT)) speed *= 3.0f;

    Vector3 move = Vector3Zero();
    if (IsKeyDown(KEY_W)) move = Vector3Add(move, Vector3Scale(forward,  speed * dt));
    if (IsKeyDown(KEY_S)) move = Vector3Add(move, Vector3Scale(forward, -speed * dt));
    if (IsKeyDown(KEY_D)) move = Vector3Add(move, Vector3Scale(right,    speed * dt));
    if (IsKeyDown(KEY_A)) move = Vector3Add(move, Vector3Scale(right,   -speed * dt));
    if (IsKeyDown(KEY_Q)) move = Vector3Add(move, Vector3Scale(up,       speed * dt));
    if (IsKeyDown(KEY_E)) move = Vector3Add(move, Vector3Scale(up,      -speed * dt));

    ctx->camera3d.position = Vector3Add(ctx->camera3d.position, move);
    ctx->camera3d.target   = Vector3Add(ctx->camera3d.position, forward);
    ctx->camera3d.up       = up;
}
