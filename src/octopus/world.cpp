#include "world.hpp"

#include <format>
#include <iostream>

void Hero::update(float delta)
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

    position += velocity * delta;
}

void Scorpion::update(float delta)
{
    position += velocity * delta;


    height = std::max(0.f, height + verticalVelocity * delta);

    verticalVelocity -= gravity * delta;
    if (height == 0.f) {
        verticalVelocity = 0.f;
    }
}

void World::update(float delta)
{
    hero.update(delta);
    for (auto& scorpion : scorpions) {
        scorpion.brain.update(delta);
        scorpion.update(delta);
    }
}

