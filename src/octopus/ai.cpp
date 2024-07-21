#include "ai.hpp"

#include "events.hpp"
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

CoroTask backAwayFrom(
    std::string name, SimpleMovementComponent& mov, const WorldPosition& point)
{
    std::cerr << "backAwayFrom: start\n";

    static constexpr float backAwayDistance = 10.f;
    static constexpr float proximity = 0.3f;

    auto direction = (mov.position - point).norm();
    auto targetPoint = mov.position + direction * backAwayDistance;
    while (distance(mov.position, targetPoint) < proximity) {
        auto movement = (targetPoint - mov.position).norm();
        mov.velocity = movement * mov.maxSpeed;
        co_await std::suspend_always{};
    }

    std::cerr << "backAwayFrom: finish\n";
}

CoroTask hiss(std::string name, Entity entity)
{
    std::cerr << "hiss: start\n";

    using namespace std::chrono_literals;

    events.push(HissEvent{entity});
    // co_await WaitFor{1s};

    std::cerr << "hiss: finish\n";
    co_return;
}

CoroTask approach(
    std::string name,
    SimpleMovementComponent& mov,
    const SmoothMovementComponent& hero,
    float targetDistance)
{
    std::cerr << "approach: start\n";

    while (distance(mov.position, hero.position) > targetDistance) {
        mov.velocity = (hero.position - mov.position).norm() * mov.maxSpeed;
        co_await std::suspend_always{};
    }

    std::cerr << "approach: finish\n";
}

CoroTask jumpAttack(
    std::string name, SimpleMovementComponent& mov, const WorldPosition& point)
{
    std::cerr << "jumpAttack: start\n";

    mov.verticalVelocity = 6.f;
    mov.velocity = (point - mov.position) / 1.5f;
    while (mov.height > 0 || mov.verticalVelocity != 0) {
        co_await std::suspend_always{};
    }

    std::cerr << "jumpAttack: finish\n";
}

CoroTask moveTo(std::string name, SimpleMovementComponent& mov, const WorldPosition& point)
{
    std::cerr << "move to: " << point << "\n";

    while (distance(mov.position, point) > 0.2) {
        std::cerr << "current distance is: " << distance(mov.position, point)
                  << "\n";

        mov.velocity = (point - mov.position).norm() * mov.maxSpeed;

        std::cerr << "awaiting inside moveTo\n";
        co_await std::suspend_always{};
        std::cerr << "resumed inside moveTo\n";
    }

    std::cerr << "target point reached\n";
}

CoroTask fidget(std::string name, SimpleMovementComponent& mov)
{
    std::cerr << "fidget\n";

    for (int i = 0; i < 3; i++) {
        auto targetPoint = randomPointInSquare(mov.position, 1);
        std::cerr << "moving to " << targetPoint << "\n";
        co_await moveTo("moveTo", mov, targetPoint);
    }
}

CoroTask think(std::string name, Ecs& ecs, Entity entity)
{
    std::cerr << "think: start\n";

    auto& ai = ecs.component<AiComponent>(entity);
    auto& mov = ecs.component<SimpleMovementComponent>(entity);

    const auto& hero = ecs.components<SmoothMovementComponent>().front();

    auto dist = std::uniform_int_distribution<int>{0, 1};

    for (;;) {
        if (ai.fear > 50) {
            if (distance(mov.position, hero.position) < 10) {
                co_await backAwayFrom("backAwayFrom", mov, hero.position);
            } else {
                co_await hiss("hiss", entity);
            }
        } else {
            if (distance(mov.position, hero.position) > 3) {
                auto action = dist(random().engine());
                std::cerr << "action id: " << action << "\n";
                if (action == 0) {
                    std::cerr << "think: start approach\n";
                    co_await approach("approach", mov, hero, 3);
                } else if (action == 1) {
                    std::cerr << "think: start fidget\n";
                    co_await fidget("fidget", mov);
                }
            } else {
                std::cerr << "think: start jump attack\n";
                co_await jumpAttack("jumpAttack", mov, hero.position);
            }
        }
    }

    std::cerr << "think: finish\n";
}
