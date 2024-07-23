#pragma once

#include <chrono>

class FrameTimer {
public:
    explicit FrameTimer(int fps);

    int operator()();
    [[nodiscard]] float delta() const;
    void relax() const;

private:
    using Clock = std::chrono::high_resolution_clock;

    Clock::duration _frameDuration;
    Clock::time_point _start = Clock::now();
    size_t _lastFrame = 0;
};
