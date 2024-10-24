#include "movement.hpp"
#include "loadobj.hpp"
#include "blitobj.hpp"
#include "game_state.hpp"
#include "game_constants.hpp"
#include <cmath>
#include <algorithm> 
#include <cstdlib>  // For rand()



int bullets = 0;
int enemybullets = 0;

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

inline void handleCollisionDamage(obj* target) {
    if (target == &player) {
        if (player.health > 0) {
            player.health -= 10;
            int new_damage_state = (1000 - player.health) / 200;
            if (new_damage_state > 5) new_damage_state = 5;
            if (new_damage_state != player.damage_state) {
                player.damage_state = new_damage_state;
                player.texture_pointer = player.damage_textures[new_damage_state];
            }
            if (player.health <= 0) player.isalive = 0;
        }
    } else if (target == &battlestar) {
        if (battlestar.health > 0) {
            battlestar.health -= 10;
            int new_damage_state = (1000 - battlestar.health) / 200;
            if (new_damage_state > 5) new_damage_state = 5;
            if (new_damage_state != battlestar.damage_state) {
                battlestar.damage_state = new_damage_state;
                battlestar.texture_pointer = battlestar.damage_textures[new_damage_state];
            }
            if (battlestar.health <= 0) {
                battlestar.isalive = 0;
                GameState::getInstance().addScore(battlestar.deathPoints);
            }
        }
    } else {
        if (target->health > 0) {
            target->health -= 10;
            int new_damage_state = (50 - target->health) / 10;
            if (new_damage_state > 5) new_damage_state = 5;
            if (new_damage_state != target->damage_state) {
                target->damage_state = new_damage_state;
                target->texture_pointer = target->damage_textures[new_damage_state];
            }
            if (target->health <= 0) {
                target->isalive = 0;
                GameState::getInstance().addScore(target->deathPoints);
            }
        }
    }
}

void moveStuff() {
    static float accumulator = 0.0f;
    accumulator += 1.0f/60.0f; // Assuming 60 FPS, adjust if needed

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
        for (int e = current_wave * ENEMIES_PER_WAVE; 
             e < (current_wave + 1) * ENEMIES_PER_WAVE && !collision; e++) {
            if (enemy[e].isalive && checkSimpleCollision(&chain[p], &enemy[e])) {
                handleCollisionDamage(&enemy[e]);
                collision = true;
            }
        }

        if (!collision && battlestar.isalive && checkSimpleCollision(&chain[p], &battlestar)) {
            handleCollisionDamage(&battlestar);
            collision = true;
        }

        if (collision) {
            chain[p].isalive = 0;
        } else {
            blitObj(chain[p]);
        }
    }

    // Move enemies smoothly
    for (int enemy_ctr = current_wave * ENEMIES_PER_WAVE; enemy_ctr < (current_wave + 1) * ENEMIES_PER_WAVE; enemy_ctr++) {
        if (enemy[enemy_ctr].isalive) {
            // Update the enemy's horizontal position
            float speed = ENEMY_BASE_SPEED + ((rand() % 10) * 0.1f);
            enemy[enemy_ctr].x -= speed;

            // Stop at the max X threshold
            if (enemy[enemy_ctr].x < MAX_X_THRESHOLD) {
                enemy[enemy_ctr].x = MAX_X_THRESHOLD;
            }

            // Vertical movement
            float fluctuation = ENEMY_AMPLITUDE * sin(ENEMY_FREQUENCY * enemy[enemy_ctr].x + enemy[enemy_ctr].phase);
            enemy[enemy_ctr].y = enemy[enemy_ctr].initialY + fluctuation;

            // Clamp Y position
            if (enemy[enemy_ctr].y < 0) enemy[enemy_ctr].y = 0;
            if (enemy[enemy_ctr].y > 480 - 64) enemy[enemy_ctr].y = 480 - 64;

            // Update texture based on current damage state
            int damage_state = (50 - enemy[enemy_ctr].health) / 10;
            if (damage_state > 5) damage_state = 5;
            enemy[enemy_ctr].texture_pointer = enemy[enemy_ctr].damage_textures[damage_state];

            blitObj(enemy[enemy_ctr]);
            
            // Shoot from this enemy
            if (rand() % 100 < 5) { // 5% chance to shoot per frame
                shootEnemyChain(enemy_ctr);
            }
        }
    }

    // Move enemy bullets
    for (int p = 0; p < MAX_NUM_ENEMY_BULLETS; p++) {
        if (!enemychain[p].isalive) continue;

        enemychain[p].x -= 15.0f;
        
        if (enemychain[p].x + enemychain[p].imgX < 0) {
            enemychain[p].isalive = 0;
            continue;
        }

        if (checkSimpleCollision(&enemychain[p], &player)) {
            handleCollisionDamage(&player);
            enemychain[p].isalive = 0;
        } else {
            blitObj(enemychain[p]);
        }
    }

    // Move battlestar
    if (battlestar.isalive) {
        if (battlestar.x > MAX_X_THRESHOLD) {
            battlestar.x -= 2.0f;
        }
        blitObj(battlestar);
    }
}

void shootChain() {
    if(bullets < MAX_NUM_BULLETS && chain[bullets].isalive == 0) {
        chain[bullets].isalive = 1;
        // Position bullet from the center of the player, subtracting half the bullet size
        chain[bullets].x = player.x - (chain[bullets].imgX / 2);
        chain[bullets].y = player.y - (chain[bullets].imgY / 2);
        
        // Add fine adjustment to make bullet come from middle of player
        chain[bullets].x += player.imgX / 2;
    }
    
    bullets++;
    if(bullets >= MAX_NUM_BULLETS) {
        bullets = 0;
    }
}

void shootEnemyChain(int enemyIndex) {
    if (enemybullets < MAX_NUM_ENEMY_BULLETS && enemychain[enemybullets].isalive == 0) {
        enemychain[enemybullets].isalive = 1;
        // Position bullet from the center of the enemy, subtracting half the bullet size
        enemychain[enemybullets].x = enemy[enemyIndex].x - (enemychain[enemybullets].imgX / 2);
        enemychain[enemybullets].y = enemy[enemyIndex].y - (enemychain[enemybullets].imgY / 2);
        
        // Add fine adjustment to make bullet come from middle of enemy
        enemychain[enemybullets].x += enemy[enemyIndex].imgX / 2;
    }
    
    enemybullets++;
    if (enemybullets >= MAX_NUM_ENEMY_BULLETS) {
        enemybullets = 0;
    }
}