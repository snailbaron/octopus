#pragma once

#include "ecs.hpp"
#include "geometry.hpp"
#include "task.hpp"

#include <vector>

struct SmoothMovementComponent {
    float timeToFullSpeed = 0.3f;
    float timeToFullStop = 0.2f;
    float maxSpeed = 5.f;

    WorldPosition position;
    WorldVector velocity;

    float height = 0.f;
    float verticalVelocity = 0.f;

    WorldVector control;

    constexpr float deceleration() const
    {
        return maxSpeed / timeToFullStop;
    }

    constexpr float acceleration() const
    {
        return deceleration() + maxSpeed / timeToFullSpeed;
    }
};

struct SimpleMovementComponent {
    WorldPosition position;
    WorldVector velocity;
    float height = 0.f;
    float verticalVelocity = 0.f;

    float maxSpeed = 4.f;
    float gravity = 9.f;
};

struct AiComponent {
    float fear = 0.f;
    WorldPosition homePoint;
    CoroTask brain;
};

struct PositionComponent {
    WorldPosition position;
    float radius = 0.f;
};

enum class ObjectType {
    Hero,
    Scorpion,
    Tree,
    Chest,
    House,
};

class World {
public:
    World();

    void update(float delta);

    WorldVector& heroControl();

private:
    Ecs _ecs;
};

