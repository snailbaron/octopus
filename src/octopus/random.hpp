#pragma once

#include <concepts>
#include <random>

class Random {
public:
    Random();

    template <std::integral T>
    T generate(T minValue, T maxValue)
    {
        return std::uniform_int_distribution<T>{minValue, maxValue}(_engine);
    }

    template <std::floating_point T>
    T generate(T minValue, T maxValue)
    {
        return std::uniform_real_distribution<T>{minValue, maxValue}(_engine);
    }

    std::mt19937& engine()
    {
        return _engine;
    }

private:
    std::mt19937 _engine;
};

Random& random();

template <class T>
requires std::integral<T> || std::floating_point<T>
T random(const T& minValue, const T& maxValue)
{
    return random().generate<T>(minValue, maxValue);
}