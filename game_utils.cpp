#include "game_utils.hpp"

std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();

float getDeltaTime() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    duration<float> delta = now - lastFrameTime;
    lastFrameTime = now;
    return delta.count(); // Return time in seconds
}