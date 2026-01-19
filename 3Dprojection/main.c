#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include <stdio.h>

#define W_WIDTH 800
#define W_HEIGHT 800
#define W_FPS 60
#define P_SIZE 10
#define BACKGROUND_COLOR BLACK
#define FOREGROUND GREEN

typedef struct {
    int x;
    int y;
} CanvasPoint;

typedef struct {
    double x;
    double y;
} ScreenPoint;

typedef struct {
    double x;
    double y;
    double z;
} SpacePoint;

void point(CanvasPoint p) {
    DrawRectangle(p.x - P_SIZE / 2, p.y - P_SIZE / 2, P_SIZE, P_SIZE, FOREGROUND);
}

void line(CanvasPoint p1, CanvasPoint p2) {
    DrawLine(p1.x, p1.y, p2.x, p2.y, FOREGROUND);
}

void clear() {
    ClearBackground(BACKGROUND_COLOR);
}

CanvasPoint screen(ScreenPoint sp) {
    return (CanvasPoint){
        .x = (sp.x + 1.0) / 2 * W_WIDTH,
        .y = (1.0 - (sp.y + 1.0) / 2) * W_HEIGHT};
}

ScreenPoint project(SpacePoint sp) {
    return (ScreenPoint){
        .x = sp.x / sp.z,
        .y = sp.y / sp.z};
}

SpacePoint translate_z(SpacePoint sp, double dz) {
    sp.z += dz;
    return sp;
}

SpacePoint rotate_xz(SpacePoint sp, double angle) {
    const double cos_angle = cos(angle);
    const double sin_angle = sin(angle);
    SpacePoint result = sp;

    result.x = sp.x * cos_angle - sp.z * sin_angle;
    result.z = sp.x * sin_angle + sp.z * cos_angle;
    return result;
}

static double dz = 1.0;
static double angle = 1.0;
const SpacePoint VS[] = {
    {0.25, 0.25, 0.25},
    {-0.25, 0.25, 0.25},
    {-0.25, -0.25, 0.25},
    {0.25, -0.25, 0.25},

    {0.25, 0.25, -0.25},
    {-0.25, 0.25, -0.25},
    {-0.25, -0.25, -0.25},
    {+0.25, -0.25, -0.25},
};

const int FS[][4] = {
    {0, 1, 2, 3},
    {4, 5, 6, 7},
    {0, 4, -1, -1},
    {1, 5, -1, -1},
    {2, 6, -1, -1},
    {3, 7, -1, -1}};

const int VS_SIZE = sizeof(VS) / sizeof(SpacePoint);
const int FS_SIZE1 = 4;
const int FS_SIZE2 = sizeof(FS) / (FS_SIZE1 * sizeof(int));

void frame() {
    clear();
    float dt = GetFrameTime();
    // dz += 1.0 * dt;
    angle += PI * dt;
    // for (int i = 0; i < VS_SIZE; ++i) {
    //     point(screen(project(translate_z(rotate_xz(VS[i], angle), dz))));
    // }
    for (int i = 0; i < FS_SIZE2; ++i) {
        for (int j = 0; j < FS_SIZE1; ++j) {
            if (FS[i][j + 1] < 0) {
                break;
            }
            const SpacePoint p1 = VS[FS[i][j]];
            const SpacePoint p2 = VS[FS[i][(j + 1) % FS_SIZE1]];
            line(
                screen(project(translate_z(rotate_xz(p1, angle), dz))),
                screen(project(translate_z(rotate_xz(p2, angle), dz))));
        }
    }
}

int main() {
    InitWindow(W_WIDTH, W_HEIGHT, "2dto3d");
    SetTargetFPS(W_FPS);
    printf("FS_SIZE2:%d\n ", FS_SIZE2);
    while (!WindowShouldClose()) {
        BeginDrawing();
        clear();
        DrawFPS(10, 10);
        frame();
        EndDrawing();
    }
    return 0;
}
