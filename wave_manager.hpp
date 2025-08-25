#ifndef WAVE_MANAGER_HPP
#define WAVE_MANAGER_HPP

#include "loadobj.hpp"
#include "game_constants.hpp"

class WaveManager {
public:
    WaveManager();
    
    void initializeCurrentWave();
    bool isCurrentWaveDefeated();
    void updateWaves();
    
    int getCurrentWave() const;
    bool isInBossPhase() const;
    bool isGameCompleted() const;
    void resetToFirstWave();
    
    // Query methods for other systems
    int getWaveStartIndex() const;
    int getWaveEndIndex() const;
    bool shouldShowBattlestar() const;
    
private:
    int current_wave;
    int enemies_defeated;
    bool wave_in_progress;
    bool game_completed;
    
    static const int TOTAL_WAVES;
    
    void spawnEnemiesForCurrentWave();
    void spawnBattlestar();
    bool areWaveEnemiesDefeated();
    bool isBattlestarDefeated();
    void advanceToNextWave();
    void completeGame();
};

#endif // WAVE_MANAGER_HPP