#include "movement.hpp"
#include "loadobj.hpp"
#include "blitobj.hpp"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Define hitbox offsets and dimensions
#define ENEMY_HITBOX_OFFSET_X 11
#define ENEMY_HITBOX_OFFSET_Y 22
#define ENEMY_HITBOX_WIDTH (52 - 11)
#define ENEMY_HITBOX_HEIGHT (41 - 22)


int bullets = 0;
int p = 0;

#define DEBUG_COLLISIONS 0  // Set to 1 to enable debug output

int checkCollision(obj* obj1, obj* obj2) {
    if (!obj1->isalive || !obj2->isalive) {
        return 0;  // No collision if either object is not alive
    }

    // Calculate the edges of obj1's hitbox
    float obj1_left = obj1->x + obj1->hitbox_offset_x;
    float obj1_right = obj1_left + obj1->hitbox_width;
    float obj1_top = obj1->y + obj1->hitbox_offset_y;
    float obj1_bottom = obj1_top + obj1->hitbox_height;

    // Calculate the edges of obj2's hitbox
    float obj2_left = obj2->x + obj2->hitbox_offset_x;
    float obj2_right = obj2_left + obj2->hitbox_width;
    float obj2_top = obj2->y + obj2->hitbox_offset_y;
    float obj2_bottom = obj2_top + obj2->hitbox_height;

    // Check for collision
    if (obj1_left < obj2_right && obj1_right > obj2_left &&
        obj1_top < obj2_bottom && obj1_bottom > obj2_top) {
        
        // Collision detected
        if (obj2 == &player) {
            // Player hit by enemy bullet
            if (player.health > 0) {
                player.health -= 10;
                
                // Update player damage state
                int new_damage_state = (1000 - player.health) / 200;
                if (new_damage_state > 5) new_damage_state = 5;
                
                if (new_damage_state != player.damage_state) {
                    player.damage_state = new_damage_state;
                    player.texture_pointer = player.damage_textures[new_damage_state];
                }
                
                if (player.health <= 0) {
                    player.isalive = 0;
                }
            }
        } else if (obj2 == &battlestar) {
            // Battlestar hit by player bullet
            if (battlestar.health > 0) {
                battlestar.health -= 10;
                
                // Update battlestar damage state
                int new_damage_state = (1000 - battlestar.health) / 200;
                if (new_damage_state > 5) new_damage_state = 5;
                
                if (new_damage_state != battlestar.damage_state) {
                    battlestar.damage_state = new_damage_state;
                    battlestar.texture_pointer = battlestar.damage_textures[new_damage_state];
                }
                
                if (battlestar.health <= 0) {
                    battlestar.isalive = 0;
                }
            }
        } else {
            // Regular enemy hit by player bullet
            if (obj2->health > 0) {
                obj2->health -= 10;
                
                // Update enemy damage state
                int new_damage_state = (50 - obj2->health) / 10;
                if (new_damage_state > 5) new_damage_state = 5;
                
                if (new_damage_state != obj2->damage_state) {
                    obj2->damage_state = new_damage_state;
                    obj2->texture_pointer = obj2->damage_textures[new_damage_state];
                }
                
                if (obj2->health <= 0) {
                    obj2->isalive = 0;
                }
            }
        }
        return 1;
    }

    // No collision
    return 0;
}

void moveStuff(){
    if(battlestar.x >= (SCREEN_WIDTH - battlestar.imgX)){
        battlestar.x = battlestar.x - 5;
    }

        for (int p = 0; p < MAX_NUM_BULLETS; p++) {
        if (chain[p].isalive == 1) {
            chain[p].x += 10 * 1.5F;
            if (chain[p].x > SCREEN_WIDTH) {
                chain[p].isalive = 0;
            } else {
                int collision = 0;
                for (int e = 0; e < MAX_NUM_ENEMIES; e++) {
                    if (checkCollision(&chain[p], &enemy[e])) {
                        collision = 1;
                        enemy[e].health -= 10;
                        if (enemy[e].health <= 0) {
                            enemy[e].isalive = 0;
                        }
                        break;  // Exit the loop once a collision is found
                    }
                    collision = 0;
                    if (checkCollision(&chain[p], &battlestar)) {
                        collision = 1;
                        battlestar.health -= 10;
                        if (battlestar.health <= 0) {
                            battlestar.isalive = 0;
                        }
                        break;  // Exit the loop once a collision is found
                    }
                }
                chain[p].isalive = !collision;
                if (chain[p].isalive) {
                    blitObj(chain[p]);
                }
            }
        }
    }

    for (int p = 0; p < MAX_NUM_ENEMY_BULLETS; p++) {
        if (enemychain[p].isalive == 1) {
            enemychain[p].x -= 10 * 1.5F;
            if (enemychain[p].x + enemychain[p].imgX < 0) {
                enemychain[p].isalive = 0;
            } else {
                if (checkCollision(&enemychain[p], &player)) {
                    enemychain[p].isalive = 0;
                    player.health -= 10;
                    if (player.health <= 0) {
                        player.isalive = 0;
                    }
                }
                if (enemychain[p].isalive) {
                    blitObj(enemychain[p]);
                }
            }
        }
    }    
}

void shootChain() {
	if(bullets < MAX_NUM_BULLETS && chain[bullets].isalive == 0){
		chain[bullets].isalive = 1;
		chain[bullets].x = player.x;// + (player.imgX / 2);
		chain[bullets].y = player.y;// + 32;
	} 
	
	bullets++;
	if(bullets > MAX_NUM_BULLETS){
		bullets = 0;
	}
}

// void shootEnemyChain(int z){
// 	if(enemybullets < MAX_NUM_ENEMY_BULLETS && enemychain[enemybullets].isalive == 0){
// 		enemychain[enemybullets].isalive = 1;
// 		enemychain[enemybullets].x = enemy[z].x + 64;
// 		enemychain[enemybullets].y = enemy[z].y + (enemy[z].imgY / 2);
// 	} 
	
// 	enemybullets++;
// 	if(enemybullets > MAX_NUM_ENEMY_BULLETS){
// 		enemybullets = 0;
// 	}
    
// }

