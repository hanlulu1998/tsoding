#include "SFML/System/Clock.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <cmath>
#include <numbers>

constexpr int W_WIDTH = 800;
constexpr int W_HEIGHT = 800;
constexpr int W_FPS = 60;
#define BACKGROUND sf::Color::Black
#define FOREGROUND sf::Color::Green

using CanvasPoint = sf::Vector2f;
using ScreenPoint = sf::Vector2f;
using SpacePoint = sf::Vector3f;

void clear(sf::RenderWindow *window) {
    window->clear(BACKGROUND);
}

void line(sf::RenderWindow *window, CanvasPoint p1, CanvasPoint p2) {
    std::array line = {
        sf::Vertex{p1},
        sf::Vertex{p2}};
    window->draw(line.data(), line.size(), sf::PrimitiveType::Lines);
}

CanvasPoint screen(const ScreenPoint sp) {
    return {
        (sp.x + 1.0f) / 2 * W_WIDTH,
        (1.0f - (sp.y + 1.0f) / 2) * W_HEIGHT};
}

ScreenPoint project(const SpacePoint sp) {
    return {
        sp.x / sp.z,
        sp.y / sp.z};
}

SpacePoint translate_z(SpacePoint sp, const float dz) {
    sp.z += dz;
    return sp;
}

SpacePoint rotate_xz(const SpacePoint sp, const float angle) {
    const double cos_angle = cos(angle);
    const double sin_angle = sin(angle);
    SpacePoint result = sp;

    result.x = sp.x * cos_angle - sp.z * sin_angle;
    result.z = sp.x * sin_angle + sp.z * cos_angle;
    return result;
}

static float dz = 1.0;
static float angle = 1.0;
const SpacePoint VS[] = {
    {0.25, 0.25, 0.25},
    {-0.25, 0.25, 0.25},
    {-0.25, -0.25, 0.25},
    {0.25, -0.25, 0.25},

    {0.25, 0.25, -0.25},
    {-0.25, 0.25, -0.25},
    {-0.25, -0.25, -0.25},
    {0.25, -0.25, -0.25},
};

const int FS[][4] = {
    {0, 1, 2, 3},
    {4, 5, 6, 7},
    {0, 4, -1, -1},
    {1, 5, -1, -1},
    {2, 6, -1, -1},
    {3, 7, -1, -1}};

const int VS_SIZE = sizeof(VS) / sizeof(SpacePoint);
const int FS_SIZE1 = sizeof(FS[0]) / sizeof(int);
const int FS_SIZE2 = sizeof(FS) / sizeof(FS[0]);

static sf::Clock timer;

float get_frame_time() {
    auto time = timer.restart();
    return time.asSeconds();
}

void frame(sf::RenderWindow *window) {
    clear(window);
    float dt = get_frame_time();
    angle += std::numbers::pi_v<float> * dt;
    dz += 0.1f * dt;
    for (const auto fs : FS) {
        for (int j = 0; j < FS_SIZE1; ++j) {
            if (fs[j + 1] < 0) {
                break;
            }
            line(window,
                 screen(project(translate_z(rotate_xz(VS[fs[j]], angle), dz))),
                 screen(project(translate_z(rotate_xz(VS[fs[(j + 1) % FS_SIZE1]], angle), dz))));
        }
    }
}

int main(int argc, char **argv) {
    sf::RenderWindow window(sf::VideoMode({W_WIDTH, W_HEIGHT}), "SFML Demo");
    window.setFramerateLimit(W_FPS);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        clear(&window);
        frame(&window);
        window.display();
    }

    return 0;
}
