#pragma once

#include "ai.hpp"
#include "events.hpp"
#include "geometry.hpp"
#include "random.hpp"
#include "task.hpp"

#include <coroutine>
#include <vector>

inline Channel events;

struct Hero {
    static constexpr float timeToFullSpeed = 0.3f;
    static constexpr float timeToFullStop = 0.2f;
    static constexpr float maxSpeed = 5.f;

    static constexpr float deceleration = maxSpeed / timeToFullStop;
    static constexpr float acceleration =
        deceleration + maxSpeed / timeToFullSpeed;

    void update(float delta);

    WorldPosition position;
    WorldVector velocity;
    WorldVector control;
};

struct Scorpion {
    void update(float delta);

    WorldPosition position;
    WorldVector velocity;
    float height = 0.f;
    float verticalVelocity = 0.f;

    float maxSpeed = 4.f;
    float gravity = 9.f;

    float fear = 0.f;
    WorldPosition homePoint;
    std::chrono::high_resolution_clock::time_point targetTime;
    CoroTask brain;
};

struct World {
    World()
    {
        scorpions.emplace_back(Scorpion{
            .position = {.x = -3, .y = 2},
        });
        scorpions.back().brain = think("think", scorpions.back(), hero);
    }

    void update(float delta);

    Hero hero;
    std::vector<Scorpion> scorpions;
};

