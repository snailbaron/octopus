#include "random.hpp"

#include <algorithm>
#include <array>

#include <iostream>

namespace {

Random _random;

} // namespace

Random::Random()
{
    auto data = std::array<unsigned, std::mt19937::state_size>{};
    std::generate_n(data.begin(), data.size(), std::random_device{});
    auto seq = std::seed_seq(data.begin(), data.end());
    _engine = std::mt19937{seq};
}

Random& random()
{
    return _random;
}