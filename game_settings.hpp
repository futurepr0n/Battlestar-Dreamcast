#ifndef GAME_SETTINGS_HPP
#define GAME_SETTINGS_HPP

#include "game_constants.hpp"

enum class Difficulty {
    Easy,
    Medium,
    Hard
};

class GameSettings {
public:
    static GameSettings& getInstance() {
        static GameSettings instance;
        return instance;
    }

    // Basic getters
    int getEnemyWaves() const { return enemyWaves; }
    Difficulty getDifficulty() const { return difficulty; }
    
    // Difficulty-based settings
    int getMaxEnemyBullets() const;
    float getEnemyBaseSpeed() const;
    float getEnemyAmplitude() const;
    float getEnemyFrequency() const;
    
    // Calculated values
    int getMaxEnemies() const { return enemyWaves * ENEMIES_PER_WAVE; }

    // Setters
    void setEnemyWaves(int waves);
    void cycleDifficulty();
    void cycleDifficultyReverse();

    // Constants
    static const int MIN_ENEMY_WAVES = 2;
    static const int MAX_ENEMY_WAVES = 10;

private:
    GameSettings() : enemyWaves(2), difficulty(Difficulty::Easy) {}
    GameSettings(const GameSettings&) = delete;
    GameSettings& operator=(const GameSettings&) = delete;

    int enemyWaves;
    Difficulty difficulty;
};

#endif // GAME_SETTINGS_HPP