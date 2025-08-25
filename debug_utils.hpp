#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <iostream>
#include <chrono>

// Compile-time debug control
#ifdef DEBUG_BUILD
    #define DEBUG_ENABLED 1
#else
    #define DEBUG_ENABLED 0
#endif

// Performance-conscious debug macros
#define DEBUG_PRINT(msg) do { if (DEBUG_ENABLED) { std::cout << msg << std::endl; } } while(0)
#define DEBUG_PRINTF(fmt, ...) do { if (DEBUG_ENABLED) { printf(fmt, __VA_ARGS__); } } while(0)

// Conditional debug functions that compile to nothing in release builds
template<typename... Args>
inline void debugLog(Args&&... args) {
    if constexpr (DEBUG_ENABLED) {
        ((std::cout << args << " "), ...);
        std::cout << std::endl;
    }
}

// Performance measurement utilities
class PerformanceTimer {
public:
    PerformanceTimer(const char* name) : timerName(name) {
        if constexpr (DEBUG_ENABLED) {
            startTime = std::chrono::steady_clock::now();
        }
    }
    
    ~PerformanceTimer() {
        if constexpr (DEBUG_ENABLED) {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            printf("Timer [%s]: %ld μs\n", timerName, duration.count());
        }
    }
    
private:
    const char* timerName;
    std::chrono::steady_clock::time_point startTime;
};

// Macro for easy performance timing
#define PERF_TIMER(name) // Disabled for performance

// Frame rate counter
class FrameRateCounter {
public:
    static FrameRateCounter& getInstance() {
        static FrameRateCounter instance;
        return instance;
    }
    
    void update();
    float getFPS() const;
    void printFPS();
    
private:
    FrameRateCounter() : frameCount(0), lastTime(0), currentFPS(0.0f) {}
    
    int frameCount;
    uint64_t lastTime;
    float currentFPS;
};

#endif // DEBUG_UTILS_HPP