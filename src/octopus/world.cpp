#include "world.hpp"

#include "ai.hpp"
#include "events.hpp"

#include <format>
#include <iostream>

void updateHero(Ecs& ecs, float delta)
{
    for (auto& entity : ecs.entities<SmoothMovementComponent>()) {
        auto& c = ecs.component<SmoothMovementComponent>(entity);

        c.velocity += c.acceleration() * c.control * delta;

        auto sqSpeed = c.velocity.sqLength();
        if (sqSpeed > 0) {
            auto speed = std::sqrt(sqSpeed);

            auto desiredSpeed = std::max(0.f, speed - c.deceleration() * delta);
            if (desiredSpeed > c.maxSpeed) {
                desiredSpeed = c.maxSpeed;
            }

            c.velocity *= desiredSpeed / speed;
        }

        c.position += c.velocity * delta;

        events.push(MoveObjectEvent{
            .id = entity,
            .position = c.position,
            .height = c.height,
        });
    }
}

void updateBrains(Ecs& ecs, float delta)
{
    for (auto& ai : ecs.components<AiComponent>()) {
        ai.brain.update(delta);
    }
}

void updateEnemies(Ecs& ecs, float delta)
{
    for (auto entity : ecs.entities<SimpleMovementComponent>()) {
        auto& mov = ecs.component<SimpleMovementComponent>(entity);

        mov.position += mov.velocity * delta;

        mov.height = std::max(0.f, mov.height + mov.verticalVelocity * delta);
        mov.verticalVelocity -= mov.gravity * delta;
        if (mov.height == 0.f) {
            mov.verticalVelocity = 0.f;
        }

        events.push(MoveObjectEvent{
            .id = entity,
            .position = mov.position,
            .height = mov.height,
        });
    }
}

World::World()
{
    auto hero = _ecs.create();
    _ecs.add(
        hero,
        SmoothMovementComponent{
            .position = {0, 0},
        });
    events.push(AddObjectEvent{
        .id = hero,
        .type = ObjectType::Hero,
    });

    auto scorpion = _ecs.create();
    _ecs.add(
        scorpion,
        SimpleMovementComponent{
            .position = {-5, 3},
        });
    _ecs.add(
        scorpion,
        AiComponent{
            .homePoint = {-5, 3},
            .brain = think("think", _ecs, scorpion),
        });
    events.push(AddObjectEvent{
        .id = scorpion,
        .type = ObjectType::Scorpion,
    });

    auto tree = _ecs.create();
    _ecs.add(
        tree,
        PositionComponent{
            .position = {3, 2},
            .radius = 1,
        });
    events.push(AddObjectEvent{
        .id = tree,
        .type = ObjectType::Tree,
        .position = {3, 2},
    });

    auto chest = _ecs.create();
    _ecs.add(
        chest,
        PositionComponent{
            .position = {4, -3},
            .radius = 1,
        });
    events.push(AddObjectEvent{
        .id = chest,
        .type = ObjectType::Chest,
        .position = {4, -3},
    });

    auto house = _ecs.create();
    _ecs.add(
        house,
        PositionComponent{
            .position = {1, -5},
        });
    events.push(AddObjectEvent{
        .id = house,
        .type = ObjectType::House,
        .position = {-2, -4},
    });
}

void World::update(float delta)
{
    updateHero(_ecs, delta);
    updateBrains(_ecs, delta);
    updateEnemies(_ecs, delta);
}

WorldVector& World::heroControl()
{
    return _ecs.components<SmoothMovementComponent>().front().control;
}
