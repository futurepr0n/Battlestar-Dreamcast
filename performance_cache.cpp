#include "performance_cache.hpp"
#include "debug_utils.hpp"
#include <algorithm>

PerformanceCache::PerformanceCache() {
    calculateDamageThresholds();
    DEBUG_PRINT("PerformanceCache: Initialized with pre-calculated values");
}

void PerformanceCache::calculateDamageThresholds() {
    // Pre-calculate player damage thresholds (1000 HP, 6 states)
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        damageThresholds.player[i] = 1000 - (i * 200);
    }
    
    // Pre-calculate enemy damage thresholds (50 HP, 6 states)  
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        damageThresholds.enemy[i] = 50 - (i * 10);
    }
    
    // Pre-calculate battlestar damage thresholds (1000 HP, 6 states)
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        damageThresholds.battlestar[i] = 1000 - (i * 200);
    }
}

int PerformanceCache::getPlayerDamageState(int health) const {
    // Fast lookup instead of division
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        if (health > damageThresholds.player[i]) {
            return std::max(0, i - 1);
        }
    }
    return MAX_DAMAGE_STATES - 1;
}

int PerformanceCache::getEnemyDamageState(int health) const {
    // Fast lookup instead of division
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        if (health > damageThresholds.enemy[i]) {
            return std::max(0, i - 1);
        }
    }
    return MAX_DAMAGE_STATES - 1;
}

int PerformanceCache::getBattlestarDamageState(int health) const {
    // Fast lookup instead of division
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        if (health > damageThresholds.battlestar[i]) {
            return std::max(0, i - 1);
        }
    }
    return MAX_DAMAGE_STATES - 1;
}

float PerformanceCache::fastSqrt(float x) const {
    // Use hardware sqrt if available, otherwise fall back to standard
    return sqrtf(x);
}

float PerformanceCache::fastDistance(float dx, float dy) const {
    // For collision detection, we often only need relative distances
    // Manhattan distance is faster and often sufficient
    return fabsf(dx) + fabsf(dy);
}