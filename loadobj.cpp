// loadobj.cpp DREAMCAST: Blitting a player
//
////////////////////////////////////////////////////////////////////////////
#include <kos.h>
#include <iostream>
#include <cstdlib>  // For rand()
#include <ctime>
#include <png/png.h>
#include "loadobj.hpp"
#include "movement.hpp"

// Global texture pointers for enemy damage states
pvr_ptr_t g_enemy_textures[MAX_DAMAGE_STATES] = {nullptr};
pvr_ptr_t g_player_textures[MAX_DAMAGE_STATES] = {nullptr};
pvr_ptr_t g_enemybattlestar_textures[MAX_DAMAGE_STATES] = {nullptr};


// Initialize new variables
int current_wave = 0;
int enemies_defeated = 0;
bool wave_in_progress = false;


obj player;
obj starfield;
obj background;
obj battlestar;
obj enemy[MAX_NUM_ENEMIES];
obj enemychain[MAX_NUM_ENEMY_BULLETS];
obj chain[MAX_NUM_BULLETS];

void loadCharacterData(){
	loadPlayer();
	loadEnemies();
	loadEnemyBattlestar();
	loadChain();
	loadEnemyChain();
	loadBackground();
	loadStarfield();
	//loadHsBar();
	//loadStatusBar();
	
}


void initializeWave() {
    if (current_wave * ENEMIES_PER_WAVE < MAX_NUM_ENEMIES) {
        for (int i = 0; i < ENEMIES_PER_WAVE; i++) {
            int index = current_wave * ENEMIES_PER_WAVE + i;
            enemy[index].isalive = 1;
            enemy[index].x = 640 + i * 100; // Start off-screen
            enemy[index].health = 50; // Reset health
            enemy[index].damage_state = 0;
            enemy[index].texture_pointer = enemy[index].damage_textures[0];
        }
        wave_in_progress = true;
    } else if (!battlestar.isalive) {
        // All waves defeated, spawn battlestar
        battlestar.isalive = 1;
        battlestar.x = 640; // Start off-screen
        battlestar.health = 1000; // Reset health
        battlestar.damage_state = 0;
        battlestar.texture_pointer = battlestar.damage_textures[0];
    }
}

bool isWaveDefeated() {
    for (int i = current_wave * ENEMIES_PER_WAVE; i < (current_wave + 1) * ENEMIES_PER_WAVE; i++) {
        if (enemy[i].isalive) {
            return false;
        }
    }
    return true;
}

void updateEnemyWaves() {
    if (!wave_in_progress) {
        initializeWave();
    } else if (isWaveDefeated()) {
        current_wave++;
        wave_in_progress = false;
        enemies_defeated += ENEMIES_PER_WAVE;
    }
}

void loadPlayerTextures() {
    const char* player_damage_image_paths[MAX_DAMAGE_STATES] = {
        "/rd/vipersm2.png",
        "/rd/vipersm2-damage-1.png",
        "/rd/vipersm2-damage-2.png",
        "/rd/vipersm2-damage-3.png",
        "/rd/vipersm2-damage-4.png",
        "/rd/vipersm2-damage-5.png"
    };

    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        g_player_textures[i] = pvr_mem_malloc(64 * 64 * 2);
        if (!g_player_textures[i]) {
            printf("Failed to allocate memory for player damage texture %d\n", i);
            continue;
        }
        png_to_texture(player_damage_image_paths[i], g_player_textures[i], PNG_FULL_ALPHA);
    }
}

void loadPlayer(){
	
	player.imgX = 64;
	player.imgY = 64 ;
    
    player.x = 320; // Half of 640 (screen width)
    player.y = 240; // Half of 480 (screen height)
    player.isalive = 1;
	player.hitbox_offset_x = 8;
	player.hitbox_offset_y = 9;
	player.hitbox_width = (52-8);
	player.hitbox_height = (43-9);
	player.health = 1000;
	player.damage_state = 0;

	
    loadPlayerTextures();

    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        player.damage_textures[i] = g_player_textures[i];
    }

    player.texture_pointer = player.damage_textures[0];
}

void unloadPlayerTextures() {
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        if (g_player_textures[i]) {
            pvr_mem_free(g_player_textures[i]);
            g_player_textures[i] = nullptr;
        }
    }
}


void loadChain() {
    for(int i = 0; i < MAX_NUM_BULLETS; i++) {
        chain[i].x = 0;
        chain[i].y = 0;
        chain[i].imgX = 8;
        chain[i].imgY = 8;
        // Center the hitbox
        chain[i].hitbox_offset_x = 0;
        chain[i].hitbox_offset_y = 0;
        chain[i].hitbox_width = 8;
        chain[i].hitbox_height = 8;
        chain[i].texture_pointer = pvr_mem_malloc(8 * 8 * 2);
        if(!chain[i].texture_pointer) {
            printf("chain image failed to load...");
        }
        png_to_texture("/rd/chain.png", chain[i].texture_pointer, PNG_FULL_ALPHA);
        chain[i].isalive = 0;
        chain[i].pctr = 0;
    }
}

void loadEnemyTextures() {
    const char* damage_image_paths[MAX_DAMAGE_STATES] = {
        "/rd/raidersm.png",
        "/rd/raidersm-damage-1.png",
        "/rd/raidersm-damage-2.png",
        "/rd/raidersm-damage-3.png",
        "/rd/raidersm-damage-4.png",
        "/rd/raidersm-damage-5.png"
    };

    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        g_enemy_textures[i] = pvr_mem_malloc(64 * 64 * 2);
        if (!g_enemy_textures[i]) {
            printf("Failed to allocate memory for enemy damage texture %d\n", i);
            // Handle error (e.g., reduce texture quality or use fewer damage states)
            continue;
        }
        png_to_texture(damage_image_paths[i], g_enemy_textures[i], PNG_FULL_ALPHA);
    }
}

void loadEnemies() {
    std::srand(std::time(nullptr));
    loadEnemyTextures();

    for (int i = 0; i < MAX_NUM_ENEMIES; i++) {
        enemy[i].x = 640 + (i % ENEMIES_PER_WAVE) * 100;
        enemy[i].y = 100 + (i % ENEMIES_PER_WAVE) * 60;
        enemy[i].imgX = 64;
        enemy[i].imgY = 64;
        enemy[i].health = 50;
        // Center the hitbox
        enemy[i].hitbox_offset_x = 16;  // (64 - 32) / 2
        enemy[i].hitbox_offset_y = 16;  // (64 - 32) / 2
        enemy[i].hitbox_width = 32;     // Adjust based on actual sprite
        enemy[i].hitbox_height = 32;    // Adjust based on actual sprite
        enemy[i].deathPoints = 10;
        enemy[i].initialY = enemy[i].y;
        enemy[i].wave_number = i / ENEMIES_PER_WAVE;
        enemy[i].frequency = 0.02f + (rand() % 10) * 0.01f;
        enemy[i].amplitude = 10.0f + (rand() % 30);
        enemy[i].phase = rand() % 360;
        enemy[i].isalive = 0;
        
        // Setup textures
        for (int j = 0; j < MAX_DAMAGE_STATES; j++) {
            enemy[i].damage_textures[j] = g_enemy_textures[j];
        }
        enemy[i].texture_pointer = enemy[i].damage_textures[0];
        enemy[i].damage_state = 0;
    }
}

// Add this function to your loadobj.cpp
void unloadEnemyTextures() {
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        if (g_enemy_textures[i]) {
            pvr_mem_free(g_enemy_textures[i]);
            g_enemy_textures[i] = nullptr;
        }
    }
}

void loadEnemyChain(){
	int i;	
	for(i = 0; i < MAX_NUM_ENEMY_BULLETS; i++){
		enemychain[i].x = 0;  // enemy[i].x;
		enemychain[i].y = 0;  // enemy[i].y;
		enemychain[i].imgX = 8;
		enemychain[i].imgY = 8;
		enemychain[i].hitbox_offset_x = 0;
		enemychain[i].hitbox_offset_y = 0;
		enemychain[i].hitbox_width = 8;
		enemychain[i].hitbox_height = 8;
		enemychain[i].texture_pointer = pvr_mem_malloc(8 * 8 * 2);
		if(!enemychain[i].texture_pointer){
			printf("enemychain image failed to load...");
		}
		png_to_texture("/rd/enemychain.png", enemychain[i].texture_pointer, PNG_FULL_ALPHA);
		enemychain[i].isalive = 0;
		enemychain[i].pctr = 0;
	}
}


void loadEnemyBattlestarTextures() {
    const char* damage_image_paths[MAX_DAMAGE_STATES] = {
        "/rd/battlestar.png",
        "/rd/battlestar-damage-1.png",
        "/rd/battlestar-damage-2.png",
        "/rd/battlestar-damage-3.png",
        "/rd/battlestar-damage-4.png",
        "/rd/battlestar-damage-5.png"
    };
	
	for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        g_enemybattlestar_textures[i] = pvr_mem_malloc(128 * 128 * 2);
        if (!g_enemybattlestar_textures[i]) {
            printf("Failed to allocate memory for player damage texture %d\n", i);
            continue;
        }
        png_to_texture(damage_image_paths[i], g_enemybattlestar_textures[i], PNG_FULL_ALPHA);
    }
}


void loadEnemyBattlestar(){
	battlestar.x = 10050;
	battlestar.y = (480/2) + 17;
	battlestar.imgX = 128;
	battlestar.imgY = 128;
	battlestar.health = 1000;
	battlestar.hitbox_offset_x = 8;
	battlestar.hitbox_offset_y = 9;
	battlestar.hitbox_width = (120-8);
	battlestar.hitbox_height = (120-9);
	battlestar.damage_state = 0;
    battlestar.isalive = 1;
	battlestar.deathPoints = 100;

	loadEnemyBattlestarTextures();

    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        battlestar.damage_textures[i] = g_enemybattlestar_textures[i];
    }

    battlestar.texture_pointer = battlestar.damage_textures[0];
}

// Add this function to your loadobj.cpp
void unloadEnemyBattlestarTextures() {
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        if (g_enemybattlestar_textures[i]) {
            pvr_mem_free(g_enemybattlestar_textures[i]);
            g_enemybattlestar_textures[i] = nullptr;
        }
    }
}

void loadBackground(){
	
	background.x = 0;
	background.y = 0;
	background.imgX = 512;
	background.imgY = 512;
	//background.imgX = 480;
	//background.imgY = 272;
    
    background.texture_pointer = pvr_mem_malloc(512 * 512 * 2);

    if(!background.texture_pointer){
		printf("background image failed to load...");
	}
    png_to_texture("/rd/space_bg1.png", background.texture_pointer, PNG_NO_ALPHA);


	background.isalive = 1;
}

void loadStarfield(){
	starfield.x = 0;
	starfield.y = 0;
	starfield.imgX = 512;
	starfield.imgY = 512;


    starfield.texture_pointer = pvr_mem_malloc(512 * 512 * 2);

	if(!starfield.texture_pointer){
		printf("Starfield image failed to load...");
	}
    
    png_to_texture("/rd/starfield_one1.png", starfield.texture_pointer, PNG_MASK_ALPHA);

	starfield.isalive = 1;
}

/*
void loadStatusBar(){
	status_bar.x = -60;
	status_bar.y = 224;
	status_bar.imgX = 160;
	status_bar.imgY = 90;
	status_bar.img = oslLoadImageFilePNG("status_bar_new.png", OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_8888);
	if(!status_bar.img){
		printf("status_bar image load faild");
	}
	status_bar.isalive = 1;
}
*/

/*
void loadHsBar(){
	hs_bar.x = 0;
	hs_bar.y = 0;
	hs_bar.imgX = 480;
	hs_bar.imgY = 10;
	hs_bar.img2 = oslLoadImageFilePNG("hs_bar_clear.png", OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_8888);
	hs_bar.img = oslLoadImageFilePNG("hs_bar.png", OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_8888);
	if(!hs_bar.img){
		printf("hs_bar image failed to load...");
	}
	hs_bar.isalive = 1;
}

*/