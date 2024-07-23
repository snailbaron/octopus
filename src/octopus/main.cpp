#include "build-info.hpp"
#include "events.hpp"
#include "geometry.hpp"
#include "scene.hpp"
#include "timer.hpp"
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
    };

    auto treeTexture =
        renderer.loadTexture(build_info::assets / "images" / "tree.png");
    auto treeSize = treeTexture.size();
    auto treeSprite = Sprite{
        .texture = &treeTexture,
        .frames = {
            SDL_Rect{0, 0, treeSize.w, treeSize.h},
        },
    };

    auto chestTexture =
        renderer.loadTexture(build_info::assets / "images" / "chest.png");
    auto chestSize = chestTexture.size();
    auto chestSprite = Sprite{
        .texture = &chestTexture,
        .frames = {
            SDL_Rect{0, 0, chestSize.w, chestSize.h},
        },
    };

    auto houseTexture =
        renderer.loadTexture(build_info::assets / "images" / "house.png");
    auto houseSize = houseTexture.size();
    auto houseSprite = Sprite{
        .texture = &houseTexture,
        .frames = {
            SDL_Rect{0, 0, houseSize.w, houseSize.h},
        },
    };

    auto spriteForObject =
        [&heroSprite, &scorpionSprite, &treeSprite, &chestSprite, &houseSprite]
        (ObjectType objectType) -> const Sprite& {
            switch (objectType) {
                case ObjectType::Hero: return heroSprite;
                case ObjectType::Scorpion: return scorpionSprite;
                case ObjectType::Tree: return treeSprite;
                case ObjectType::Chest: return chestSprite;
                case ObjectType::House: return houseSprite;
            }
            throw std::runtime_error{std::format(
                "unknown ObjectType: {}", std::to_underlying(objectType))};
        };

    auto scene = Scene{};
    scene.camera().center(0.f, 0.f);
    scene.camera().viewport(0, 0, window.size().w, window.size().h);
    scene.camera().pixelsPerUnit(32);
    scene.camera().zoom(2);

    std::vector<LifeHolder> lifeHolders;

    lifeHolders.push_back(events.subscribe<AddObjectEvent>(
        [&scene, &spriteForObject] (const AddObjectEvent& e) {
            scene.addObject(e.id, spriteForObject(e.type), e.position);
        }));
    lifeHolders.push_back(events.subscribe<MoveObjectEvent>(
        [&scene] (const MoveObjectEvent& e) {
            scene.moveObject(
                e.id,
                e.position + WorldVector{0, 0.5f * e.height});
        }));

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
            world.heroControl() = controller.control();

            for (int i = 0; i < framesPassed; i++) {
                world.update(timer.delta());
            }

            events.deliver();

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