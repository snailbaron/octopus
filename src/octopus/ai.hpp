#pragma once

#include "geometry.hpp"
#include "task.hpp"

struct Scorpion;
struct Hero;

CoroTask think(std::string name, Scorpion& scorpion, const Hero& hero);
CoroTask backAwayFrom(std::string name, Scorpion& scorpion, const WorldPosition& point);
CoroTask hiss(std::string name);
CoroTask approach(std::string name, Scorpion& scorpion, const Hero& hero, float targetDistance);
CoroTask jumpAttack(std::string name, Scorpion& scorpion, const WorldPosition& point);
CoroTask moveTo(std::string name, Scorpion& scorpion, const WorldPosition& point);
CoroTask fidget(std::string name, Scorpion& scorpion);