#pragma once

#include "channel.hpp"
#include "ecs.hpp"
#include "world.hpp"

inline Channel events;

struct AddObjectEvent {
    Entity id;
    ObjectType type;
    WorldPosition position;
};

struct MoveObjectEvent {
    Entity id;
    WorldPosition position;
    float height = 0.f;
};

struct HissEvent {
    Entity entity;
};