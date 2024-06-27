#include "build-info.hpp"
#include "geometry.hpp"

#include "sdl.hpp"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <map>
#include <thread>
#include <vector>

#include <format>
#include <iostream>

using Clock = std::chrono::high_resolution_clock;

struct WorldTag;
struct ScreenTag;

using WorldPosition = Point<float, WorldTag>;
using WorldVector = Vector<float, WorldTag>;

using ScreenPosition = Point<float, ScreenTag>;
using ScreenRect = Rect<float, ScreenTag>;

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
    { }

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
    [[nodiscard]] ScreenPosition project(
        const WorldPosition& worldPosition) const
    {
        return ScreenPosition{
            .x = _viewport.center().x +
                (worldPosition.x - _center.x) * screenPixelsPerUnit(),
            .y = _viewport.center().y +
                (_center.y - worldPosition.y) * screenPixelsPerUnit(),
        };
    }

    [[nodiscard]] WorldPosition restore(
        const ScreenPosition& screenPosition) const
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

class FrameTimer {
public:
    explicit FrameTimer(int fps)
        : _frameDuration(
            std::chrono::duration_cast<Clock::duration>(
                std::chrono::duration<float>(1.f / fps)))
    { }

    int operator()()
    {
        auto currentFrame = (Clock::now() - _start) / _frameDuration;
        auto framesPassed = static_cast<int>(currentFrame - _lastFrame);
        _lastFrame = currentFrame;
        return framesPassed;
    }

    [[nodiscard]] float delta() const
    {
        return std::chrono::duration_cast<std::chrono::duration<float>>(
            _frameDuration).count();
    }

    void relax() const
    {
        auto nextFrameTime = _start + _frameDuration * (_lastFrame + 1);
        std::this_thread::sleep_until(nextFrameTime);
    }

private:
    Clock::duration _frameDuration;
    Clock::time_point _start = Clock::now();
    size_t _lastFrame = 0;
};

struct KeyboardController {
    [[nodiscard]] WorldVector control() const
    {
        return WorldVector{(float)(right - left), (float)(up - down)};
    }

    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
};

struct Hero {
    static constexpr float timeToFullSpeed = 0.3f;
    static constexpr float timeToFullStop = 0.2f;
    static constexpr float maxSpeed = 5.f;

    static constexpr float deceleration = maxSpeed / timeToFullStop;
    static constexpr float acceleration =
        deceleration + maxSpeed / timeToFullSpeed;

    void update(float delta)
    {
        velocity.x += acceleration * control.x * delta;
        velocity.y += acceleration * control.y * delta;

        auto sqSpeed = velocity.x * velocity.x + velocity.y * velocity.y;
        if (sqSpeed > 0) {
            auto speed = std::sqrt(sqSpeed);

            auto desiredSpeed = std::max(0.f, speed - deceleration * delta);
            if (desiredSpeed > maxSpeed) {
                desiredSpeed = maxSpeed;
            }

            velocity.x *= desiredSpeed / speed;
            velocity.y *= desiredSpeed / speed;
        }

        position.x += velocity.x * delta;
        position.y += velocity.y * delta;
    }

    WorldPosition position;
    WorldVector velocity;
    WorldVector control;
};

int main(int, char*[])
{
    using namespace std::chrono_literals;

    auto sdlInit = sdl::Init{SDL_INIT_VIDEO | SDL_INIT_AUDIO};
    auto imgInit = img::Init{IMG_INIT_PNG};

    auto window = sdl::Window{
        "octopus",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1024,
        768,
        0};

    auto renderer = sdl::Renderer{
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

    auto heroTexture =
        renderer.loadTexture(build_info::assets / "images" / "hero.png");
    auto heroSize = heroTexture.size();
    auto heroSprite = Sprite{
        .texture = &heroTexture,
        .frames = {
            SDL_Rect{0, 0, heroSize.w, heroSize.h},
        },
        .frameDuration = 200ms,
    };

    auto scene = Scene{};
    scene.camera().center(0.f, 0.f);
    scene.camera().viewport(0, 0, window.size().w, window.size().h);
    scene.camera().pixelsPerUnit(32);
    scene.camera().zoom(2);

    scene.addObject(0, heroSprite, {});

    auto controller = KeyboardController{};
    auto hero = Hero{};

    auto timer = FrameTimer{240};
    for (;;) {
        bool done = false;
        while (auto event = sdl::pollEvent()) {
            if (event->type == SDL_QUIT) {
                done = true;
                break;
            }

            if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
                bool pressed = (event->type == SDL_KEYDOWN);
                if (event->key.keysym.sym == SDLK_w) {
                    controller.up = pressed;
                } else if (event->key.keysym.sym == SDLK_s) {
                    controller.down = pressed;
                } else if (event->key.keysym.sym == SDLK_a) {
                    controller.left = pressed;
                } else if (event->key.keysym.sym == SDLK_d) {
                    controller.right = pressed;
                }
            }
        }

        if (done) {
            break;
        }

        if (int framesPassed = timer(); framesPassed > 0) {
            hero.control = controller.control();
            for (int i = 0; i < framesPassed; i++) {
                hero.update(timer.delta());
            }

            scene.moveObject(0, hero.position);
            scene.update((float)framesPassed * timer.delta());

            renderer.setDrawColor(50, 50, 50, 255);
            renderer.clear();

            scene.render(renderer);

            renderer.present();
        }

        timer.relax();
    }



    return EXIT_SUCCESS;
}