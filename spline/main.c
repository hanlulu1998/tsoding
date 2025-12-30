#include "da_utils.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOWS_S 200
#define WIDTH_F 4
#define HEIGHT_F 3

#define WINDOWS_W (WINDOWS_S * WIDTH_F)
#define WINDOWS_H (WINDOWS_S * HEIGHT_F)

#define GRID_S 25
#define GRID_W (GRID_S * WIDTH_F)
#define GRID_H (GRID_S * HEIGHT_F)

#define CELL_W (WINDOWS_W / GRID_W)
#define CELL_H (WINDOWS_H / GRID_H)

typedef struct {
    bool show;
    bool boundary;
} Grid;

static Grid grid[GRID_H][GRID_W] = {0};
static int dragging = -1;
typedef struct {
    Vector2 *items;
    size_t count;
    size_t capacity;
} Points;

static Points points = {0};

typedef struct {
    float tx;
    float d;
} Solution;

typedef struct {
    Solution *items;
    size_t count;
    size_t capacity;
} Solutions;

static Solutions solutions = {0};

int cmp_solutions_by_tx(const void *a, const void *b) {
    const Solution *sa = a;
    const Solution *sb = b;
    return sa->tx - sb->tx;
}

void solve_row(size_t row, Solutions *solutions) {
    solutions->count = 0;

    float y = (row + 0.5) * CELL_H;
    for (size_t i = 0; i + 2 <= points.count; i += 2) {
        Vector2 p1 = points.items[i];
        Vector2 p2 = points.items[i + 1];
        Vector2 p3 = points.items[(i + 2) % points.count];

        // 使用二次贝塞尔展开公式
        float dx12 = p2.x - p1.x;
        float dx23 = p3.x - p2.x;

        float dy12 = p2.y - p1.y;
        float dy23 = p3.y - p2.y;

        // 二元函数求根
        float a = dy23 - dy12;
        float b = 2 * dy12;
        float c = p1.y - y;

        float t[2] = {0};
        size_t tn = 0;

        if (fabsf(a) > 1e-6) {
            float D = b * b - 4 * a * c;
            if (D >= 0.0) {
                t[tn++] = (-b + sqrtf(D)) / (2 * a);
                t[tn++] = (-b - sqrtf(D)) / (2 * a);
            }
        } else if (fabsf(b) > 1e-6) {
            t[tn++] = -c / b;
        }

        for (size_t j = 0; j < tn; ++j) {
            if (!(0 <= t[j] && t[j] <= 1))
                continue;
            float tx = (dx23 - dx12) * t[j] * t[j] + 2 * dx12 * t[j] + p1.x;
            float d = (dy23 - dy12) * t[j] + dy12;
            Solution solution = {tx, d};
            DA_APPEND(*solutions, solution);
        }
    }

    qsort(solutions->items, solutions->count, sizeof(*(solutions->items)), cmp_solutions_by_tx);
}

void clear_grid() {
    for (size_t row = 0; row < GRID_H; ++row) {
        for (size_t col = 0; col < GRID_W; ++col) {
            grid[row][col] = (Grid){false, false};
        }
    }
}

void render_spline_to_grid();

void edit_control_point() {
    Vector2 mouse = GetMousePosition();

    for (size_t i = 0; i < points.count; ++i) {
        Vector2 size = {20, 20};
        Vector2 position = points.items[i];
        position = Vector2Subtract(position, Vector2Scale(size, 0.5));
        bool hover = CheckCollisionPointRec(mouse, (Rectangle){position.x, position.y, size.x, size.y});
        if (hover) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                dragging = i;
        } else {
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                dragging = -1;
        }
        DrawRectangleV(position, size, hover ? RED : BLUE);
    }

    if (dragging >= 0) {
        if (points.items[dragging].x != mouse.x || points.items[dragging].y != mouse.y) {
            render_spline_to_grid();
        }
        points.items[dragging] = mouse;
    } else {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            DA_APPEND(points, mouse);
        }
    }
}

void display_grid() {
    for (size_t y = 0; y < GRID_H; ++y) {
        for (size_t x = 0; x < GRID_W; ++x) {
            if (grid[y][x].show) {
                Vector2 mark_position = {x * CELL_W, y * CELL_H};
                Vector2 cell_size = {CELL_W, CELL_H};
                Vector2 mark_size = Vector2Scale(cell_size, 0.25);
                mark_position = Vector2Add(mark_position, Vector2Scale(cell_size, 0.5));
                mark_position =
                    Vector2Subtract(mark_position, Vector2Scale(mark_size, 0.5));
                DrawRectangleV(mark_position, mark_size, grid[y][x].boundary ? BLUE : RED);
            }
        }
    }
}

void render_spline_to_grid() {
    clear_grid();

    for (size_t row = 0; row < GRID_H; ++row) {
        int winding = 0;
        solve_row(row, &solutions);
        for (size_t i = 0; i < solutions.count; ++i) {
            Solution solution = solutions.items[i];
            if (winding > 0) {
                if (i > 0) {
                    Solution pre_solution = solutions.items[i - 1];
                    size_t col1 = pre_solution.tx / CELL_W;
                    size_t col2 = solution.tx / CELL_W;
                    grid[row][col1] = (Grid){true, true};
                    grid[row][col2] = (Grid){true,true};
                    for (size_t col = col1 + 1; col < col2; ++col) {
                        grid[row][col] = (Grid){true,false};
                    }
                }
            }

            if (solution.d > 0) {
                winding += 1;
            } else if (solution.d < 0) {
                winding -= 1;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    InitWindow(WINDOWS_W, WINDOWS_H, "Splines");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x181818FF));
        if (IsKeyPressed(KEY_F2)) {
            points.count = 0;
            clear_grid();
        }
        edit_control_point();
        display_grid();
        EndDrawing();
    }

    DA_CLEAR(points);
    DA_CLEAR(solutions);

    return 0;
}
