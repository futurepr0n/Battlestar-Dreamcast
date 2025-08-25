#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include "input_system.hpp"
#include "wave_manager.hpp"
#include "bullet_manager.hpp"
#include "font_manager.hpp"
#include "debug_utils.hpp"
#include "performance_cache.hpp"

// Unified game engine that coordinates all systems
class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    bool initialize();
    void cleanup();
    
    void update();
    void render();
    
    bool isGameCompleted() const;
    void resetGame();
    
    // Performance monitoring
    void updatePerformanceStats();
    void printPerformanceReport();
    
private:
    InputSystem inputSystem;
    WaveManager waveManager;
    BulletManager bulletManager;
    
    // Singletons (managed externally)
    FontManager& fontManager;
    PerformanceCache& perfCache;
    FrameRateCounter& fpsCounter;
    
    bool initialized;
    
    // Performance tracking
    int frameCount;
    float averageFrameTime;
};

#endif // GAME_ENGINE_HPP