#include "game_settings.hpp"
#include <algorithm>

void GameSettings::setEnemyWaves(int waves) {
    enemyWaves = std::clamp(waves, MIN_ENEMY_WAVES, MAX_ENEMY_WAVES);
}

void GameSettings::cycleDifficulty() {
    switch (difficulty) {
        case Difficulty::Easy:
            difficulty = Difficulty::Medium;
            break;
        case Difficulty::Medium:
            difficulty = Difficulty::Hard;
            break;
        case Difficulty::Hard:
            difficulty = Difficulty::Easy;
            break;
    }
}

void GameSettings::cycleDifficultyReverse() {
    switch (difficulty) {
        case Difficulty::Easy:
            difficulty = Difficulty::Hard;
            break;
        case Difficulty::Medium:
            difficulty = Difficulty::Easy;
            break;
        case Difficulty::Hard:
            difficulty = Difficulty::Medium;
            break;
    }
}

int GameSettings::getMaxEnemyBullets() const {
    switch (difficulty) {
        case Difficulty::Easy:
            return 1;
        case Difficulty::Medium:
            return 2;
        case Difficulty::Hard:
            return ABSOLUTE_MAX_ENEMY_BULLETS;
        default:
            return 1;
    }
}

float GameSettings::getEnemyBaseSpeed() const {
    switch (difficulty) {
        case Difficulty::Easy:
            return 2.0f;
        case Difficulty::Medium:
            return 3.0f;
        case Difficulty::Hard:
            return 5.0f;
        default:
            return 2.0f;
    }
}

float GameSettings::getEnemyAmplitude() const {
    switch (difficulty) {
        case Difficulty::Easy:
            return 10.0f;
        case Difficulty::Medium:
            return 15.0f;
        case Difficulty::Hard:
            return 20.0f;
        default:
            return 10.0f;
    }
}

float GameSettings::getEnemyFrequency() const {
    switch (difficulty) {
        case Difficulty::Easy:
            return 0.03f;
        case Difficulty::Medium:
            return 0.05f;
        case Difficulty::Hard:
            return 0.07f;
        default:
            return 0.03f;
    }
}