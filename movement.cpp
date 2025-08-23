#include "movement.hpp"
#include "loadobj.hpp"
#include "blitobj.hpp"
#include "game_state.hpp"
#include "game_constants.hpp"
#include "game_settings.hpp"
#include <cmath>
#include <algorithm> 
#include <cstdlib>  // For rand()

int bullets = 0;
int enemybullets = 0;
const int TOTAL_WAVES = ABSOLUTE_MAX_ENEMIES / ENEMIES_PER_WAVE;

inline bool checkSimpleCollision(obj* obj1, obj* obj2) {
    if (!obj1->isalive || !obj2->isalive) return false;

    // Calculate centers of objects using their actual positions and dimensions
    float obj1_center_x = obj1->x;  // Position is already center-based
    float obj1_center_y = obj1->y;
    float obj2_center_x = obj2->x;
    float obj2_center_y = obj2->y;

    // Calculate collision boundaries using half-dimensions
    float obj1_half_width = obj1->imgX / 2;
    float obj1_half_height = obj1->imgY / 2;
    float obj2_half_width = obj2->imgX / 2;
    float obj2_half_height = obj2->imgY / 2;

    // Calculate distances between centers
    float dx = fabs(obj1_center_x - obj2_center_x);
    float dy = fabs(obj1_center_y - obj2_center_y);

    // Check for overlap using actual sprite dimensions
    return dx < (obj1_half_width + obj2_half_width) &&
           dy < (obj1_half_height + obj2_half_height);
}

void handleCollisionDamage(obj* target) {
    if (!target->isalive) return;

    if (target == &player) {
        if (player.health > 0) {
            player.health -= 10;
            int new_damage_state = (1000 - player.health) / 200;
            new_damage_state = std::min(new_damage_state, MAX_DAMAGE_STATES - 1);
            
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
    else if (target == &battlestar && current_wave == TOTAL_WAVES) {
        if (battlestar.health > 0) {
            battlestar.health -= 10;
            int new_damage_state = (1000 - battlestar.health) / 200;
            new_damage_state = std::min(new_damage_state, MAX_DAMAGE_STATES - 1);
            
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
            int new_damage_state = (50 - target->health) / 10;
            new_damage_state = std::min(new_damage_state, MAX_DAMAGE_STATES - 1);
            
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

void moveStuff() {
    auto& settings = GameSettings::getInstance();
    static float accumulator = 0.0f;
    accumulator += 1.0f/60.0f;

    // Move bullets
    for (int p = 0; p < MAX_NUM_BULLETS; p++) {
        if (!chain[p].isalive) continue;
        
        chain[p].x += 15.0f;
        
        if (chain[p].x > SCREEN_WIDTH) {
            chain[p].isalive = 0;
            continue;
        }

        bool collision = false;
        // Check collisions with active enemies in current wave
        if (current_wave < TOTAL_WAVES) {
            for (int e = current_wave * ENEMIES_PER_WAVE; 
                 e < (current_wave + 1) * ENEMIES_PER_WAVE && !collision; e++) {
                if (enemy[e].isalive && checkSimpleCollision(&chain[p], &enemy[e])) {
                    handleCollisionDamage(&enemy[e]);
                    collision = true;
                }
            }
        }

        // Check battlestar collision only when it's the active boss
        if (!collision && current_wave == TOTAL_WAVES && battlestar.isalive && 
            checkSimpleCollision(&chain[p], &battlestar)) {
            handleCollisionDamage(&battlestar);
            collision = true;
        }

        if (collision) {
            chain[p].isalive = 0;
        } else {
            blitObj(chain[p]);
        }
    }

    // Move enemies only if we're not in battlestar phase
    int currentWaveEnemies = settings.getEnemyWaves() * ENEMIES_PER_WAVE;
    for (int enemy_ctr = current_wave * ENEMIES_PER_WAVE; 
         enemy_ctr < std::min(currentWaveEnemies, ABSOLUTE_MAX_ENEMIES) && 
         enemy_ctr < (current_wave + 1) * ENEMIES_PER_WAVE; enemy_ctr++) {
        if (enemy[enemy_ctr].isalive) {
            float speed = settings.getEnemyBaseSpeed();
            enemy[enemy_ctr].x -= speed;

            // Stop at the max X threshold
            if (enemy[enemy_ctr].x < MAX_X_THRESHOLD) {
                enemy[enemy_ctr].x = MAX_X_THRESHOLD;
            }

            // Vertical movement
            float fluctuation = settings.getEnemyAmplitude() * 
                        sin(settings.getEnemyFrequency() * enemy[enemy_ctr].x + 
                            enemy[enemy_ctr].phase);
            enemy[enemy_ctr].y = enemy[enemy_ctr].initialY + fluctuation;

            // Clamp Y position
            if (enemy[enemy_ctr].y < 0) enemy[enemy_ctr].y = 0;
            if (enemy[enemy_ctr].y > SCREEN_HEIGHT - enemy[enemy_ctr].imgY) 
                enemy[enemy_ctr].y = SCREEN_HEIGHT - enemy[enemy_ctr].imgY;

            // Update texture based on current damage state
            if (enemy[enemy_ctr].damage_state >= 0 && 
                enemy[enemy_ctr].damage_state < MAX_DAMAGE_STATES) {
                enemy[enemy_ctr].texture_pointer = 
                    enemy[enemy_ctr].damage_textures[enemy[enemy_ctr].damage_state];
            }

            blitObj(enemy[enemy_ctr]);
            
            // Shoot from this enemy
            if (rand() % 100 < 5) { // 5% chance to shoot per frame
                shootEnemyChain(enemy_ctr);
            }
        }
    }

    // Move enemy bullets
    int maxEnemyBullets = settings.getMaxEnemyBullets();
for (int p = 0; p < std::min(maxEnemyBullets, ABSOLUTE_MAX_ENEMY_BULLETS); p++) {
    if (!enemychain[p].isalive) continue;

    // Just use the pre-calculated velocities, no recalculation
    enemychain[p].x += enemychain[p].velocity_x;
    enemychain[p].y += enemychain[p].velocity_y;
    
    if (enemychain[p].x + enemychain[p].imgX < 0 || 
        enemychain[p].x > SCREEN_WIDTH ||
        enemychain[p].y + enemychain[p].imgY < 0 || 
        enemychain[p].y > SCREEN_HEIGHT) {
        enemychain[p].isalive = 0;
        continue;
    }

    if (player.isalive && checkSimpleCollision(&enemychain[p], &player)) {
        handleCollisionDamage(&player);
        enemychain[p].isalive = 0;
    } else {
        // Use the pre-calculated rotation
        blitObj(enemychain[p], enemychain[p].rotation);
    }
}

    // Move battlestar only when it's the active boss
    if (current_wave == TOTAL_WAVES && battlestar.isalive) {
        // Move towards threshold position
        if (battlestar.x > MAX_X_THRESHOLD) {
            battlestar.x -= 2.0f;
        }

        // Ensure battlestar texture is valid before rendering
        if (battlestar.texture_pointer && 
            battlestar.damage_state >= 0 && 
            battlestar.damage_state < MAX_DAMAGE_STATES) {
            
            battlestar.texture_pointer = 
                battlestar.damage_textures[battlestar.damage_state];
            blitObj(battlestar);

            // Battlestar shooting logic
            if (rand() % 100 < 2) { // 2% chance to shoot per frame
                shootEnemyChain(-1); // Special value for battlestar
            }
        } else {
            printf("Warning: Invalid battlestar texture state detected\n");
        }
    }
}

void shootChain() {
    if(bullets < MAX_NUM_BULLETS && chain[bullets].isalive == 0) {
        chain[bullets].isalive = 1;
        chain[bullets].x = player.x - (chain[bullets].imgX / 2);
        chain[bullets].y = player.y - (chain[bullets].imgY / 2);
        chain[bullets].x += player.imgX / 2;
    }
    
    bullets++;
    if(bullets >= MAX_NUM_BULLETS) {
        bullets = 0;
    }
}

void shootEnemyChain(int enemyIndex) {
    auto& settings = GameSettings::getInstance();
    int maxEnemyBullets = std::min(settings.getMaxEnemyBullets(), ABSOLUTE_MAX_ENEMY_BULLETS);

    if (enemybullets < maxEnemyBullets && enemychain[enemybullets].isalive == 0) {
        enemychain[enemybullets].isalive = 1;

        // Position chain exactly as before
        if (enemyIndex == -1) {  // Battlestar case
            enemychain[enemybullets].x = battlestar.x - ((enemychain[enemybullets].imgX / 2) + 64);
            enemychain[enemybullets].y = battlestar.y - (enemychain[enemybullets].imgY / 2);
            enemychain[enemybullets].x += battlestar.imgX / 2;
        } else {  // Normal enemy case
            enemychain[enemybullets].x = enemy[enemyIndex].x - ((enemychain[enemybullets].imgX / 2) + 32 );
            enemychain[enemybullets].y = enemy[enemyIndex].y - (enemychain[enemybullets].imgY / 2);
            enemychain[enemybullets].x += enemy[enemyIndex].imgX / 2;
        }

        // Get shooter position for trajectory calculation
        float shooter_x = enemychain[enemybullets].x;
        float shooter_y = enemychain[enemybullets].y;

        // Get player center Y for comparison
        float player_center_y = player.y + (player.imgY / 2);

        // Calculate target based on player position
        float target_x, target_y;
        if (player_center_y <= shooter_y) {
            target_x = player.x + player.imgX;
            target_y = player_center_y;
        } else {
            target_x = player.x;
            target_y = player_center_y;
        }

        float dx = target_x - shooter_x;
        float dy = target_y - shooter_y;

        float length = sqrtf(dx * dx + dy * dy);
        float speed = 5.0f;

        if (length > 0) {
            enemychain[enemybullets].velocity_x = (dx / length) * speed;
            enemychain[enemybullets].velocity_y = (dy / length) * speed;
            float angle = std::atan2(dy, dx) * 180.0f / M_PI;
            enemychain[enemybullets].rotation = angle < 0 ? angle + 360.0f : angle;
        } else {
            enemychain[enemybullets].velocity_x = speed;
            enemychain[enemybullets].velocity_y = 0;
            enemychain[enemybullets].rotation = 0;
        }

        enemybullets++;
        if (enemybullets >= maxEnemyBullets) {
            enemybullets = 0;
        }
    }
}