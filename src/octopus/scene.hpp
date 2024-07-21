#pragma once

#include "geometry.hpp"

#include "sdl.hpp"

#include <chrono>
#include <map>

using Clock = std::chrono::high_resolution_clock;

struct Sprite {
    sdl::Texture* texture = nullptr;
    std::vector<SDL_Rect> frames;
    Clock::duration frameDuration;
};

class Object {
public:
    Object() = default;
    Object(Sprite sprite, const WorldPosition& position);

    [[nodiscard]] const sdl::Texture& texture() const;
    [[nodiscard]] sdl::Texture& texture();
    [[nodiscard]] const SDL_Rect& frame() const;
    [[nodiscard]] const WorldPosition& position() const;

    void moveTo(const WorldPosition& position);

    void update(float delta);

private:
    Sprite _sprite;
    WorldPosition _position;
    Clock::time_point _startTime;
    Clock::time_point _currentTime;
};

class Camera {
public:
    [[nodiscard]] ScreenPosition
    project(const WorldPosition& worldPosition) const;

    [[nodiscard]] WorldPosition
    restore(const ScreenPosition& screenPosition) const;

    void viewport(int x, int y, int w, int h);
    void center(float x, float y);
    void pixelsPerUnit(float pixelsPerUnit);

    void zoom(float zoom);
    [[nodiscard]] float zoom() const;

private:
    [[nodiscard]] float screenPixelsPerUnit() const;

    ScreenRect _viewport;
    WorldPosition _center;
    float _pixelsPerUnit = 1.f;
    float _zoom = 1.f;
};

class Scene {
public:
    void addObject(size_t id, Sprite sprite, WorldPosition position);
    void moveObject(size_t id, const WorldPosition& position);
    void killObject(size_t id);
    void update(float delta);
    void render(sdl::Renderer& renderer);

    Camera& camera();

private:
    std::map<size_t, Object> _objects;
    Camera _camera;
};
