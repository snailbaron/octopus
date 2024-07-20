#include "build-info.hpp"
#include "geometry.hpp"
#include "scene.hpp"
#include "world.hpp"

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

    auto scorpionTexture =
        renderer.loadTexture(build_info::assets / "images" / "scorpion.png");
    auto scorpionSize = scorpionTexture.size();
    auto scorpionSprite = Sprite{
        .texture = &scorpionTexture,
        .frames = {
            SDL_Rect{0, 0, scorpionSize.w, scorpionSize.h},
        },
        .frameDuration = 200ms,
    };

    auto scene = Scene{};
    scene.camera().center(0.f, 0.f);
    scene.camera().viewport(0, 0, window.size().w, window.size().h);
    scene.camera().pixelsPerUnit(32);
    scene.camera().zoom(2);

    scene.addObject(0, heroSprite, {});
    scene.addObject(1, scorpionSprite, {});

    auto controller = KeyboardController{};
    auto world = World{};

    auto timer = FrameTimer{240};
    for (;;) {
        bool done = false;
        while (auto event = sdl::pollEvent()) {
            if (event->type == SDL_QUIT) {
                done = true;
                break;
            }

            if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
                const bool pressed = (event->type == SDL_KEYDOWN);
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

        if (const int framesPassed = timer(); framesPassed > 0) {
            world.hero.control = controller.control();

            for (int i = 0; i < framesPassed; i++) {
                world.update(timer.delta());
            }

            scene.moveObject(0, world.hero.position);

            const auto& scorpion = world.scorpions.front();
            scene.moveObject(1, scorpion.position + WorldVector{0, 0.5f * scorpion.height});

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