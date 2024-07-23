#pragma once

#include "ecs.hpp"
#include "task.hpp"

CoroTask think(std::string name, Ecs& ecs, Entity entity);
