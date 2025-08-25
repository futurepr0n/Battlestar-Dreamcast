#include "wave_manager.hpp"
#include "game_settings.hpp"
#include <iostream>
#include <algorithm>

// External texture arrays from loadobj.cpp
extern pvr_ptr_t g_enemybattlestar_textures[];

const int WaveManager::TOTAL_WAVES = ABSOLUTE_MAX_ENEMIES / ENEMIES_PER_WAVE;

WaveManager::WaveManager() 
    : current_wave(0), enemies_defeated(0), wave_in_progress(false), game_completed(false) {
}

void WaveManager::initializeCurrentWave() {
    printf("Current wave: %d, Total waves: %d\n", current_wave, TOTAL_WAVES);
    
    if (current_wave < TOTAL_WAVES) {
        spawnEnemiesForCurrentWave();
    } else if (current_wave == TOTAL_WAVES && !battlestar.isalive && !game_completed) {
        spawnBattlestar();
    }
}

void WaveManager::spawnEnemiesForCurrentWave() {
    printf("Initializing wave %d of %d\n", current_wave + 1, TOTAL_WAVES);
    
    for (int i = 0; i < ENEMIES_PER_WAVE; i++) {
        int index = current_wave * ENEMIES_PER_WAVE + i;
        if (index >= ABSOLUTE_MAX_ENEMIES) {
            printf("Warning: Trying to spawn enemy beyond MAX_NUM_ENEMIES\n");
            continue;
        }
        
        enemy[index].isalive = 1;
        enemy[index].x = 640 + i * 100;
        enemy[index].health = 50;
        enemy[index].damage_state = 0;
        enemy[index].texture_pointer = enemy[index].damage_textures[0];
        enemy[index].initialY = 100 + i * 60;
        enemy[index].y = enemy[index].initialY;
        
        printf("Spawned enemy %d at position (%f, %f)\n", index, enemy[index].x, enemy[index].y);
    }
    
    wave_in_progress = true;
}

void WaveManager::spawnBattlestar() {
    printf("Attempting to spawn Battlestar boss...\n");
    
    // Verify textures are valid
    bool texturesValid = true;
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        if (!battlestar.damage_textures[i]) {
            texturesValid = false;
            printf("Error: Battlestar texture %d is invalid\n", i);
            break;
        }
    }

    if (!texturesValid) {
        printf("Reloading battlestar textures...\n");
        loadEnemyBattlestarTextures();
        // Reassign textures
        for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
            battlestar.damage_textures[i] = g_enemybattlestar_textures[i];
        }
    }

    battlestar.x = 640;
    battlestar.y = (480/2) - 64;
    battlestar.health = 1000;
    battlestar.damage_state = 0;
    battlestar.texture_pointer = battlestar.damage_textures[0];
    battlestar.isalive = 1;
    
    wave_in_progress = true;
    printf("Battlestar spawned successfully at position (%f, %f)\n", 
           battlestar.x, battlestar.y);
}

bool WaveManager::isCurrentWaveDefeated() {
    if (current_wave == TOTAL_WAVES) {
        return isBattlestarDefeated();
    }
    return areWaveEnemiesDefeated();
}

bool WaveManager::areWaveEnemiesDefeated() {
    auto& settings = GameSettings::getInstance();
    int waveStartIndex = current_wave * ENEMIES_PER_WAVE;
    int waveEndIndex = std::min((current_wave + 1) * ENEMIES_PER_WAVE, settings.getMaxEnemies());
    
    for (int i = waveStartIndex; i < waveEndIndex; i++) {
        if (enemy[i].isalive) {
            return false;
        }
    }
    return true;
}

bool WaveManager::isBattlestarDefeated() {
    return !battlestar.isalive;
}

void WaveManager::updateWaves() {
    if (game_completed) {
        return;
    }

    if (!wave_in_progress) {
        initializeCurrentWave();
    } else if (isCurrentWaveDefeated()) {
        if (current_wave < TOTAL_WAVES) {
            advanceToNextWave();
        } else if (current_wave == TOTAL_WAVES && !battlestar.isalive) {
            completeGame();
        }
    }
}

void WaveManager::advanceToNextWave() {
    printf("Wave %d completed! Enemies defeated: %d\n", 
           current_wave + 1, enemies_defeated);
    current_wave++;
    wave_in_progress = false;
    enemies_defeated += ENEMIES_PER_WAVE;
}

void WaveManager::completeGame() {
    printf("Game completed! All waves and battlestar defeated!\n");
    game_completed = true;
}

int WaveManager::getCurrentWave() const {
    return current_wave;
}

bool WaveManager::isInBossPhase() const {
    return current_wave == TOTAL_WAVES;
}

bool WaveManager::isGameCompleted() const {
    return game_completed;
}

void WaveManager::resetToFirstWave() {
    current_wave = 0;
    enemies_defeated = 0;
    wave_in_progress = false;
    game_completed = false;
}

int WaveManager::getWaveStartIndex() const {
    return current_wave * ENEMIES_PER_WAVE;
}

int WaveManager::getWaveEndIndex() const {
    return std::min((current_wave + 1) * ENEMIES_PER_WAVE, ABSOLUTE_MAX_ENEMIES);
}

bool WaveManager::shouldShowBattlestar() const {
    return isInBossPhase() && battlestar.isalive;
}