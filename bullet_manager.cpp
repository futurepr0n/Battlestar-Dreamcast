#include "bullet_manager.hpp"
#include "blitobj.hpp"
#include "game_state.hpp"
#include "game_settings.hpp"
#include "performance_cache.hpp"
#include <cmath>
#include <algorithm>

BulletManager::BulletManager() : player_bullet_index(0), enemy_bullet_index(0) {
}

void BulletManager::shootPlayerBullet(const obj& player) {
    if (player_bullet_index < MAX_NUM_BULLETS && chain[player_bullet_index].isalive == 0) {
        chain[player_bullet_index].isalive = 1;
        chain[player_bullet_index].x = player.x - (chain[player_bullet_index].imgX / 2);
        chain[player_bullet_index].y = player.y - (chain[player_bullet_index].imgY / 2);
        chain[player_bullet_index].x += player.imgX / 2;
    }
    
    player_bullet_index++;
    if (player_bullet_index >= MAX_NUM_BULLETS) {
        player_bullet_index = 0;
    }
}

void BulletManager::shootEnemyBullet(int enemyIndex) {
    auto& settings = GameSettings::getInstance();
    int maxEnemyBullets = std::min(settings.getMaxEnemyBullets(), ABSOLUTE_MAX_ENEMY_BULLETS);

    if (enemy_bullet_index < maxEnemyBullets && enemychain[enemy_bullet_index].isalive == 0) {
        enemychain[enemy_bullet_index].isalive = 1;

        float shooter_x, shooter_y;
        
        // Position bullet based on shooter
        if (enemyIndex == -1) {  // Battlestar case
            enemychain[enemy_bullet_index].x = battlestar.x - ((enemychain[enemy_bullet_index].imgX / 2) + 64);
            enemychain[enemy_bullet_index].y = battlestar.y - (enemychain[enemy_bullet_index].imgY / 2);
            enemychain[enemy_bullet_index].x += battlestar.imgX / 2;
        } else {  // Normal enemy case
            enemychain[enemy_bullet_index].x = enemy[enemyIndex].x - ((enemychain[enemy_bullet_index].imgX / 2) + 32);
            enemychain[enemy_bullet_index].y = enemy[enemyIndex].y - (enemychain[enemy_bullet_index].imgY / 2);
            enemychain[enemy_bullet_index].x += enemy[enemyIndex].imgX / 2;
        }

        shooter_x = enemychain[enemy_bullet_index].x;
        shooter_y = enemychain[enemy_bullet_index].y;

        // Calculate target position based on player position
        float player_center_y = player.y + (player.imgY / 2);
        float target_x, target_y;
        
        if (player_center_y <= shooter_y) {
            target_x = player.x + player.imgX;
            target_y = player_center_y;
        } else {
            target_x = player.x;
            target_y = player_center_y;
        }

        calculateBulletTrajectory(enemychain[enemy_bullet_index], shooter_x, shooter_y, target_x, target_y);
        
        enemy_bullet_index++;
        if (enemy_bullet_index >= maxEnemyBullets) {
            enemy_bullet_index = 0;
        }
    }
}

void BulletManager::calculateBulletTrajectory(obj& bullet, float shooter_x, float shooter_y, 
                                              float target_x, float target_y) {
    float dx = target_x - shooter_x;
    float dy = target_y - shooter_y;
    float length = sqrtf(dx * dx + dy * dy);
    float speed = 5.0f;

    if (length > 0) {
        bullet.velocity_x = (dx / length) * speed;
        bullet.velocity_y = (dy / length) * speed;
        float angle = std::atan2(dy, dx) * 180.0f / M_PI;
        bullet.rotation = angle < 0 ? angle + 360.0f : angle;
    } else {
        bullet.velocity_x = speed;
        bullet.velocity_y = 0;
        bullet.rotation = 0;
    }
}

void BulletManager::updateBullets(const WaveManager& waveManager) {
    // Disabled spatial grid for performance - use direct collision detection
    updatePlayerBullets(waveManager);
    updateEnemyBullets();
}

void BulletManager::buildSpatialGrid(const WaveManager& waveManager) {
    spatialGrid.clear();
    
    // Add active enemies to spatial grid
    if (waveManager.getCurrentWave() < ABSOLUTE_MAX_ENEMIES / ENEMIES_PER_WAVE) {
        int waveStart = waveManager.getWaveStartIndex();
        int waveEnd = waveManager.getWaveEndIndex();
        
        for (int e = waveStart; e < waveEnd; e++) {
            if (enemy[e].isalive) {
                spatialGrid.addObject(&enemy[e]);
            }
        }
    }
    
    // Add battlestar if active
    if (waveManager.shouldShowBattlestar()) {
        spatialGrid.addObject(&battlestar);
    }
    
    // Add player for enemy bullet collisions
    if (player.isalive) {
        spatialGrid.addObject(&player);
    }
}

void BulletManager::updatePlayerBullets(const WaveManager& waveManager) {
    for (int p = 0; p < MAX_NUM_BULLETS; p++) {
        if (!chain[p].isalive) continue;
        
        chain[p].x += 15.0f;
        
        if (chain[p].x > SCREEN_WIDTH) {
            chain[p].isalive = 0;
            continue;
        }

        bool collision = false;
        
        // Direct collision detection - check enemies in current wave
        if (waveManager.getCurrentWave() < ABSOLUTE_MAX_ENEMIES / ENEMIES_PER_WAVE) {
            int waveStart = waveManager.getWaveStartIndex();
            int waveEnd = waveManager.getWaveEndIndex();
            
            for (int e = waveStart; e < waveEnd; e++) {
                if (enemy[e].isalive && checkSimpleCollision(&chain[p], &enemy[e])) {
                    handleCollisionDamage(&enemy[e]);
                    collision = true;
                    break;
                }
            }
        }
        
        // Check battlestar collision
        if (!collision && waveManager.shouldShowBattlestar() && 
            battlestar.isalive && checkSimpleCollision(&chain[p], &battlestar)) {
            handleCollisionDamage(&battlestar);
            collision = true;
        }

        if (collision) {
            chain[p].isalive = 0;
        } else {
            blitObj(chain[p]);
        }
    }
}

void BulletManager::updateEnemyBullets() {
    auto& settings = GameSettings::getInstance();
    int maxEnemyBullets = std::min(settings.getMaxEnemyBullets(), ABSOLUTE_MAX_ENEMY_BULLETS);
    
    for (int p = 0; p < maxEnemyBullets; p++) {
        if (!enemychain[p].isalive) continue;

        enemychain[p].x += enemychain[p].velocity_x;
        enemychain[p].y += enemychain[p].velocity_y;
        
        if (enemychain[p].x + enemychain[p].imgX < 0 || 
            enemychain[p].x > SCREEN_WIDTH ||
            enemychain[p].y + enemychain[p].imgY < 0 || 
            enemychain[p].y > SCREEN_HEIGHT) {
            enemychain[p].isalive = 0;
            continue;
        }

        // Use spatial grid for enemy bullet collision with player
        std::vector<obj*> potentialTargets = spatialGrid.getPotentialCollisions(&enemychain[p]);
        bool collision = false;
        
        for (obj* target : potentialTargets) {
            if (target == &player && target->isalive && checkSimpleCollision(&enemychain[p], target)) {
                handleCollisionDamage(target);
                collision = true;
                break;
            }
        }
        
        if (collision) {
            enemychain[p].isalive = 0;
        } else {
            blitObj(enemychain[p], enemychain[p].rotation);
        }
    }
}

bool BulletManager::checkSimpleCollision(const obj* obj1, const obj* obj2) {
    if (!obj1->isalive || !obj2->isalive) return false;

    float obj1_center_x = obj1->x;
    float obj1_center_y = obj1->y;
    float obj2_center_x = obj2->x;
    float obj2_center_y = obj2->y;

    float obj1_half_width = obj1->imgX / 2;
    float obj1_half_height = obj1->imgY / 2;
    float obj2_half_width = obj2->imgX / 2;
    float obj2_half_height = obj2->imgY / 2;

    float dx = fabs(obj1_center_x - obj2_center_x);
    float dy = fabs(obj1_center_y - obj2_center_y);

    return dx < (obj1_half_width + obj2_half_width) &&
           dy < (obj1_half_height + obj2_half_height);
}

void BulletManager::handleCollisionDamage(obj* target) {
    if (!target->isalive) return;

    // Disabled PerformanceCache for debugging

    if (target == &player) {
        if (player.health > 0) {
            player.health -= 10;
            int new_damage_state = (1000 - player.health) / 200; // Direct calculation
            
            if (new_damage_state != player.damage_state) {
                player.damage_state = new_damage_state;
                if (player.damage_textures[new_damage_state]) {
                    player.texture_pointer = player.damage_textures[new_damage_state];
                }
            }
            
            if (player.health <= 0) {
                player.isalive = 0;
            }
        }
    } 
    else if (target == &battlestar) {
        if (battlestar.health > 0) {
            battlestar.health -= 10;
            int new_damage_state = (1000 - battlestar.health) / 200; // Direct calculation
            
            if (new_damage_state != battlestar.damage_state) {
                battlestar.damage_state = new_damage_state;
                if (battlestar.damage_textures[new_damage_state]) {
                    battlestar.texture_pointer = battlestar.damage_textures[new_damage_state];
                }
            }
            
            if (battlestar.health <= 0) {
                battlestar.isalive = 0;
                GameState::getInstance().addScore(battlestar.deathPoints);
            }
        }
    } 
    else {
        // Regular enemies
        if (target->health > 0) {
            target->health -= 10;
            int new_damage_state = (100 - target->health) / 20; // Direct calculation
            
            if (new_damage_state != target->damage_state) {
                target->damage_state = new_damage_state;
                if (target->damage_textures[new_damage_state]) {
                    target->texture_pointer = target->damage_textures[new_damage_state];
                }
            }
            
            if (target->health <= 0) {
                target->isalive = 0;
                GameState::getInstance().addScore(target->deathPoints);
            }
        }
    }
}

void BulletManager::resetAllBullets() {
    player_bullet_index = 0;
    enemy_bullet_index = 0;
    
    for (int i = 0; i < MAX_NUM_BULLETS; i++) {
        chain[i].isalive = 0;
    }
    for (int i = 0; i < ABSOLUTE_MAX_ENEMY_BULLETS; i++) {
        enemychain[i].isalive = 0;
    }
}