#pragma once

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

    Object(Sprite sprite, const WorldPosition& position)
        : _sprite(std::move(sprite))
        , _position(position)
        , _startTime(Clock::now())
        , _currentTime(_startTime)
    {}

    [[nodiscard]] const sdl::Texture& texture() const
    {
        return *_sprite.texture;
    }

    [[nodiscard]] sdl::Texture& texture()
    {
        return *_sprite.texture;
    }

    [[nodiscard]] const SDL_Rect& frame() const
    {
        auto framesPassed = (_currentTime - _startTime) / _sprite.frameDuration;
        auto frameIndex = framesPassed % _sprite.frames.size();
        return _sprite.frames.at(frameIndex);
    }

    void update(float delta)
    {
        _currentTime += std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<float>(delta));
    }

    [[nodiscard]] const WorldPosition& position() const
    {
        return _position;
    }

    void moveTo(const WorldPosition& position)
    {
        _position = position;
    }

private:
    Sprite _sprite;
    WorldPosition _position;
    Clock::time_point _startTime;
    Clock::time_point _currentTime;
};

class Camera {
public:
    [[nodiscard]] ScreenPosition
    project(const WorldPosition& worldPosition) const
    {
        return ScreenPosition{
            .x = _viewport.center().x +
                 (worldPosition.x - _center.x) * screenPixelsPerUnit(),
            .y = _viewport.center().y +
                 (_center.y - worldPosition.y) * screenPixelsPerUnit(),
        };
    }

    [[nodiscard]] WorldPosition
    restore(const ScreenPosition& screenPosition) const
    {
        return WorldPosition{
            .x = _center.x + (screenPosition.x - _viewport.center().x) /
                                 screenPixelsPerUnit(),
            .y = _center.y + (_viewport.center().y - screenPosition.y) /
                                 screenPixelsPerUnit(),
        };
    }

    void viewport(int x, int y, int w, int h)
    {
        _viewport = ScreenRect{
            .x = (float)x,
            .y = (float)y,
            .w = (float)w,
            .h = (float)h,
        };
    }

    void center(float x, float y)
    {
        _center = WorldPosition{x, y};
    }

    void pixelsPerUnit(float pixelsPerUnit)
    {
        _pixelsPerUnit = pixelsPerUnit;
    }

    void zoom(float zoom)
    {
        _zoom = zoom;
    }

    [[nodiscard]] float zoom() const
    {
        return _zoom;
    }

private:
    [[nodiscard]] float screenPixelsPerUnit() const
    {
        return _pixelsPerUnit * _zoom;
    }

    ScreenRect _viewport;
    WorldPosition _center;
    float _pixelsPerUnit = 1.f;
    float _zoom = 1.f;
};

class Scene {
public:
    void addObject(size_t id, Sprite sprite, WorldPosition position)
    {
        _objects[id] = Object{std::move(sprite), position};
    }

    void moveObject(size_t id, const WorldPosition& position)
    {
        _objects.at(id).moveTo(position);
    }

    void killObject(size_t id)
    {
        _objects.erase(id);
    }

    void update(float delta)
    {
        for (auto& [id, object] : _objects) {
            object.update(delta);
        }
    }

    void render(sdl::Renderer& renderer)
    {
        for (auto& [id, object] : _objects) {
            auto screenPosition = _camera.project(object.position());

            renderer.copy(
                object.texture(),
                object.frame(),
                SDL_FRect{
                    .x = screenPosition.x -
                         _camera.zoom() * (float)object.frame().w / 2.f,
                    .y = screenPosition.y -
                         _camera.zoom() * (float)object.frame().h / 2.f,
                    .w = _camera.zoom() * (float)object.frame().w,
                    .h = _camera.zoom() * (float)object.frame().h,
                });
        }
    }

    Camera& camera()
    {
        return _camera;
    }

private:
    std::map<size_t, Object> _objects;
    Camera _camera;
};
