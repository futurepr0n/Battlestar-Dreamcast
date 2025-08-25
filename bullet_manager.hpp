#ifndef BULLET_MANAGER_HPP
#define BULLET_MANAGER_HPP

#include "loadobj.hpp"
#include "game_constants.hpp"
#include "wave_manager.hpp"
#include "spatial_grid.hpp"

class BulletManager {
public:
    BulletManager();
    
    void shootPlayerBullet(const obj& player);
    void shootEnemyBullet(int enemyIndex);  // -1 for battlestar
    void updateBullets(const WaveManager& waveManager);
    void resetAllBullets();
    
private:
    int player_bullet_index;
    int enemy_bullet_index;
    SpatialGrid spatialGrid;
    
    void updatePlayerBullets(const WaveManager& waveManager);
    void updateEnemyBullets();
    void buildSpatialGrid(const WaveManager& waveManager);
    
    bool checkSimpleCollision(const obj* obj1, const obj* obj2);
    void handleCollisionDamage(obj* target);
    
    // Helper methods for enemy bullet targeting
    void calculateBulletTrajectory(obj& bullet, float shooter_x, float shooter_y, 
                                   float target_x, float target_y);
};

#endif // BULLET_MANAGER_HPP