#include "debug_utils.hpp"
#include <kos.h>

void FrameRateCounter::update() {
    if constexpr (!DEBUG_ENABLED) {
        return; // Compile out in release builds
    }
    
    frameCount++;
    uint64_t currentTime = timer_ms_gettime64();
    
    if (lastTime == 0) {
        lastTime = currentTime;
        return;
    }
    
    uint64_t elapsed = currentTime - lastTime;
    if (elapsed >= 1000) { // Update every second
        currentFPS = (frameCount * 1000.0f) / elapsed;
        frameCount = 0;
        lastTime = currentTime;
    }
}

float FrameRateCounter::getFPS() const {
    return currentFPS;
}

void FrameRateCounter::printFPS() {
    if constexpr (DEBUG_ENABLED) {
        static int printCounter = 0;
        printCounter++;
        
        // Print FPS every 60 frames to avoid spam
        if (printCounter >= 60) {
            printf("FPS: %.1f\n", currentFPS);
            printCounter = 0;
        }
    }
}