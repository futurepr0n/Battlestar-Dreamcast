#include "game_engine.hpp"
#include "renderer.hpp"
#include "movement.hpp"
#include "audio_system.h"
#include "loadobj.hpp"

GameEngine::GameEngine() 
    : fontManager(FontManager::getInstance())
    , perfCache(PerformanceCache::getInstance())
    , fpsCounter(FrameRateCounter::getInstance())
    , initialized(false)
    , frameCount(0)
    , averageFrameTime(0.0f) {
}

GameEngine::~GameEngine() {
    cleanup();
}

bool GameEngine::initialize() {
    if (initialized) {
        return true;
    }
    
    DEBUG_PRINT("GameEngine: Initializing all systems...");
    
    // Initialize font system
    if (!fontManager.initialize()) {
        DEBUG_PRINT("GameEngine: Failed to initialize font system");
        return false;
    }
    
    // Initialize audio system
    if (!g_audioSystem.initialize("/rd/mrdeath.xm")) {
        DEBUG_PRINT("GameEngine: Failed to initialize audio system");
        return false;
    }
    
    // Reset all game systems to clean state
    resetGame();
    
    initialized = true;
    DEBUG_PRINT("GameEngine: All systems initialized successfully");
    return true;
}

void GameEngine::cleanup() {
    if (!initialized) {
        return;
    }
    
    DEBUG_PRINT("GameEngine: Cleaning up all systems...");
    
    g_audioSystem.cleanup();
    fontManager.cleanup();
    
    initialized = false;
    DEBUG_PRINT("GameEngine: Cleanup complete");
}

void GameEngine::update() {
    if (!initialized) {
        return;
    }
    
    PERF_TIMER("GameEngine::update");
    
    // Update frame counter for performance monitoring
    fpsCounter.update();
    frameCount++;
    
    // Handle player input
    inputSystem.handlePlayerInput(player);
    if (inputSystem.shouldShoot()) {
        bulletManager.shootPlayerBullet(player);
    }
    
    // Update game systems
    g_audioSystem.update();
    waveManager.updateWaves();
    bulletManager.updateBullets(waveManager);
    
    // Update legacy movement system (TODO: integrate with new systems)
    moveStuff();
}

void GameEngine::render() {
    if (!initialized) {
        return;
    }
    
    PERF_TIMER("GameEngine::render");
    
    // Call existing rendering system
    draw_scene();
}

bool GameEngine::isGameCompleted() const {
    return waveManager.isGameCompleted();
}

void GameEngine::resetGame() {
    DEBUG_PRINT("GameEngine: Resetting game state");
    
    waveManager.resetToFirstWave();
    bulletManager.resetAllBullets();
    resetGameState(); // Legacy function
    
    frameCount = 0;
    averageFrameTime = 0.0f;
}

void GameEngine::updatePerformanceStats() {
    // Performance monitoring - only in debug builds
    if constexpr (DEBUG_ENABLED) {
        fpsCounter.printFPS();
    }
}

void GameEngine::printPerformanceReport() {
    if constexpr (DEBUG_ENABLED) {
        printf("=== Performance Report ===\n");
        printf("Frames rendered: %d\n", frameCount);
        printf("Current FPS: %.1f\n", fpsCounter.getFPS());
        printf("Average frame time: %.2f ms\n", averageFrameTime);
        printf("==========================\n");
    }
}