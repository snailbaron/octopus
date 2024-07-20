#include "ai.hpp"

#include "random.hpp"
#include "world.hpp"

#include <chrono>
#include <tuple>
#include <utility>

#include <iostream>

namespace {

WorldPosition randomPointInSquare(const WorldPosition& center, float offset)
{
    auto dx = random<float>(-offset, offset);
    auto dy = random<float>(-offset, offset);
    return center + WorldVector{dx, dy};
}

} // namespace

CoroTask think(std::string name, Scorpion& scorpion, const Hero& hero)
{
    std::cerr << "think: start\n";

    auto dist = std::uniform_int_distribution<int>{0, 1};

    for (;;) {
        if (scorpion.fear > 50) {
            if (distance(scorpion.position, hero.position) < 10) {
                co_await backAwayFrom("backAwayFrom", scorpion, hero.position);
            } else {
                co_await hiss("hiss");
            }
        } else {
            if (distance(scorpion.position, hero.position) > 3) {
                auto action = dist(random().engine());
                std::cerr << "action id: " << action << "\n";
                if (action == 0) {
                    std::cerr << "think: start approach\n";
                    co_await approach("approach", scorpion, hero, 3);
                } else if (action == 1) {
                    std::cerr << "think: start fidget\n";
                    co_await fidget("fidget", scorpion);
                }
            } else {
                std::cerr << "think: start jump attack\n";
                co_await jumpAttack("jumpAttack", scorpion, hero.position);
            }
        }
    }

    std::cerr << "think: finish\n";
}

CoroTask backAwayFrom(std::string name, Scorpion& scorpion, const WorldPosition& point)
{
    std::cerr << "backAwayFrom: start\n";

    static constexpr float backAwayDistance = 10.f;
    static constexpr float proximity = 0.3f;

    auto direction = (scorpion.position - point).norm();
    auto targetPoint = scorpion.position + direction * backAwayDistance;
    while (distance(scorpion.position, targetPoint) < proximity) {
        auto movement = (targetPoint - scorpion.position).norm();
        scorpion.velocity = movement * scorpion.maxSpeed;
        co_await std::suspend_always{};
    }

    std::cerr << "backAwayFrom: finish\n";
}

CoroTask hiss(std::string name)
{
    std::cerr << "hiss: start\n";

    using namespace std::chrono_literals;
    co_await WaitFor{1s};

    std::cerr << "hiss: finish\n";
}

CoroTask approach(
    std::string name,
    Scorpion& scorpion,
    const Hero& hero,
    float targetDistance)
{
    //std::cerr << "approach: start\n";

    while (distance(scorpion.position, hero.position) > targetDistance) {
        scorpion.velocity =
            (hero.position - scorpion.position).norm() * scorpion.maxSpeed;
        co_await std::suspend_always{};
    }

    //std::cerr << "approach: finish\n";
}

CoroTask jumpAttack(std::string name, Scorpion& scorpion, const WorldPosition& point)
{
    std::cerr << "jumpAttack: start\n";

    scorpion.verticalVelocity = 6.f;
    scorpion.velocity = (point - scorpion.position) / 1.5f;
    while (scorpion.height > 0 || scorpion.verticalVelocity != 0) {
        co_await std::suspend_always{};
    }

    std::cerr << "jumpAttack: finish\n";
}

CoroTask moveTo(std::string name, Scorpion& scorpion, const WorldPosition& point)
{
    std::cerr << "move to: " << point << "\n";

    while (distance(scorpion.position, point) > 0.2) {
        std::cerr << "current distance is: " <<
            distance(scorpion.position, point) << "\n";

        scorpion.velocity =
            (point - scorpion.position).norm() * scorpion.maxSpeed;

        std::cerr << "awaiting inside moveTo\n";
        co_await std::suspend_always{};
        std::cerr << "resumed inside moveTo\n";
    }

    std::cerr << "target point reached\n";
}

CoroTask fidget(std::string name, Scorpion& scorpion)
{
    std::cerr << "fidget\n";

    for (int i = 0; i < 3; i++) {
        auto targetPoint = randomPointInSquare(scorpion.position, 1);
        std::cerr << "moving to " << targetPoint << "\n";
        co_await moveTo("moveTo", scorpion, targetPoint);
    }
}