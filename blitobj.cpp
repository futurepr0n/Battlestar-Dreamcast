#include <kos.h>
#include <cstdlib>  // For rand()
#include "blitobj.hpp"
#include "loadobj.hpp"
#include <cmath>

// Constants for movement
const float ENEMY_BASE_SPEED = 2.0f; // Base speed in pixels per frame
const float ENEMY_AMPLITUDE = 10.0f; // Amplitude for up and down movement
const float ENEMY_FREQUENCY = 0.05f; // Frequency for up and down movement



int enemybullets = 0;

int nextPowerOf2(int num) {
    int power = 1;
    while (power < num) {
        power <<= 1;  // Multiply by 2
    }
    return power;
}

void blitObj(const obj& object) {    
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

     // Calculate the next power of 2 for texture width and height
    float texture_width = static_cast<float>(nextPowerOf2(static_cast<int>(object.imgX)));
    float texture_height = static_cast<float>(nextPowerOf2(static_cast<int>(object.imgY)));

    // Update texture size to next power of 2
    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, (int)texture_width, (int)texture_height, object.texture_pointer, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;

    // Set half dimensions using the original image size
    float half_width = object.imgX / 2.0f;
    float half_height = object.imgY / 2.0f;

    // Calculate UV coordinates based on the original image size and texture size
    float u_max = object.imgX / texture_width;
    float v_max = object.imgY / texture_height;

    // Top-left vertex
    vert.x = object.x - half_width;
    vert.y = object.y - half_height;
    vert.z = 1.0f;
    vert.u = 0.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Top-right vertex
    vert.x = object.x + half_width;
    vert.y = object.y - half_height;
    vert.u = u_max;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-left vertex
    vert.x = object.x - half_width;
    vert.y = object.y + half_height;
    vert.u = 0.0f;
    vert.v = v_max;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-right vertex
    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.x = object.x + half_width;
    vert.y = object.y + half_height;
    vert.u = u_max;
    vert.v = v_max;
    pvr_prim(&vert, sizeof(vert));
    
}


// void shootEnemyChain(int z){
// 	if(enemybullets < MAX_NUM_ENEMY_BULLETS && enemychain[enemybullets].isalive == 0){
// 		enemychain[enemybullets].isalive = 1;
// 		enemychain[enemybullets].x = enemy[z].x;
// 		enemychain[enemybullets].y = enemy[z].y + (enemy[z].imgY / 2);
// 	} 
	
// 	enemybullets++;
// 	if(enemybullets > MAX_NUM_ENEMY_BULLETS){
// 		enemybullets = 0;
// 	}
    
// }

void shootEnemyChain(int z) {
    // Check if the enemy is visible on screen
    if (enemy[z].x >= 0 && enemy[z].x < 640 &&
        enemy[z].y >= 0 && enemy[z].y < 480) {
        
        if (enemybullets < MAX_NUM_ENEMY_BULLETS && enemychain[enemybullets].isalive == 0) {
            enemychain[enemybullets].isalive = 1;
            enemychain[enemybullets].x = enemy[z].x;
            enemychain[enemybullets].y = enemy[z].y + (enemy[z].imgY / 2);
            
            enemybullets++;
            if (enemybullets >= MAX_NUM_ENEMY_BULLETS) {
                enemybullets = 0;
            }
        }
    }
}

void blitEnemies(float deltaTime) {
    for (int enemy_ctr = 0; enemy_ctr < MAX_NUM_ENEMIES; enemy_ctr++) {
        if (enemy[enemy_ctr].isalive) {
            // Update the enemy's horizontal position
            float speed = ENEMY_BASE_SPEED + ((rand() % 10) * 0.1f);
            enemy[enemy_ctr].x -= speed * deltaTime;

            // Vertical movement
            const float maxFluctuation = 50.0f;
            float fluctuation = maxFluctuation * sin(enemy[enemy_ctr].frequency * enemy[enemy_ctr].x + enemy[enemy_ctr].phase);
            enemy[enemy_ctr].y = enemy[enemy_ctr].initialY + fluctuation;

            // Clamp Y position
            if (enemy[enemy_ctr].y < 0) enemy[enemy_ctr].y = 0;
            if (enemy[enemy_ctr].y > 480 - 64) enemy[enemy_ctr].y = 480 - 64;

            if (enemy[enemy_ctr].x <= 0) {
                enemy[enemy_ctr].isalive = 0;
            } else {
                blitObj(enemy[enemy_ctr]);
                
                // Shoot from this enemy
                if (rand() % 100 < 5) { // 5% chance to shoot per frame
                    shootEnemyChain(enemy_ctr);
                }
            }
        } else {
            // Free up memory.. do not blit to screen
            enemy[enemy_ctr].x = 500;
        }
    }
}

