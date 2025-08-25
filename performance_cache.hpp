#ifndef PERFORMANCE_CACHE_HPP
#define PERFORMANCE_CACHE_HPP

#include "game_constants.hpp"
#include <cmath>

class PerformanceCache {
public:
    static PerformanceCache& getInstance() {
        static PerformanceCache instance;
        return instance;
    }
    
    // Pre-calculated damage state thresholds
    struct DamageThresholds {
        int player[MAX_DAMAGE_STATES];
        int enemy[MAX_DAMAGE_STATES];
        int battlestar[MAX_DAMAGE_STATES];
    };
    
    const DamageThresholds& getDamageThresholds() const { return damageThresholds; }
    
    // Pre-calculated diagonal compensation
    float getDiagonalCompensation() const { return DIAGONAL_COMPENSATION; }
    
    // Fast damage state calculation (using pre-calculated thresholds)
    int getPlayerDamageState(int health) const;
    int getEnemyDamageState(int health) const;
    int getBattlestarDamageState(int health) const;
    
    // Math utilities with caching
    float fastSqrt(float x) const;
    float fastDistance(float dx, float dy) const;
    
private:
    PerformanceCache();
    
    DamageThresholds damageThresholds;
    
    void calculateDamageThresholds();
};

#endif // PERFORMANCE_CACHE_HPP