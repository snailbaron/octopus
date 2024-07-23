#include "timer.hpp"

#include <thread>

FrameTimer::FrameTimer(int fps)
    : _frameDuration(std::chrono::duration_cast<Clock::duration>(
          std::chrono::duration<float>(1.f / fps)))
{ }

int FrameTimer::operator()()
{
    auto currentFrame = (Clock::now() - _start) / _frameDuration;
    auto framesPassed = static_cast<int>(currentFrame - _lastFrame);
    _lastFrame = currentFrame;
    return framesPassed;
}

float FrameTimer::delta() const
{
    return std::chrono::duration_cast<std::chrono::duration<float>>(
               _frameDuration)
        .count();
}

void FrameTimer::relax() const
{
    auto nextFrameTime = _start + _frameDuration * (_lastFrame + 1);
    std::this_thread::sleep_until(nextFrameTime);
}
