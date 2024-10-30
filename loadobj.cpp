// loadobj.cpp DREAMCAST: Blitting a player
//
////////////////////////////////////////////////////////////////////////////
#include <kos.h>
#include <iostream>
#include <cstdlib>  // For rand()
#include <ctime>
#include <png/png.h>
#include "game_constants.hpp"
#include "loadobj.hpp"
#include "movement.hpp"
#include "game_settings.hpp"

// Global texture pointers for enemy damage states
pvr_ptr_t g_enemy_textures[MAX_DAMAGE_STATES] = {nullptr};
pvr_ptr_t g_player_textures[MAX_DAMAGE_STATES] = {nullptr};
pvr_ptr_t g_enemybattlestar_textures[MAX_DAMAGE_STATES] = {nullptr};


// Initialize new variables
int current_wave = 0;
int enemies_defeated = 0;
bool wave_in_progress = false;
bool gameCompleted = false;

const int TOTAL_WAVES = ABSOLUTE_MAX_ENEMIES / ENEMIES_PER_WAVE;  // 50/5 = 10 waves



obj player;
obj starfield;
obj background;
obj battlestar;
obj enemy[ABSOLUTE_MAX_ENEMIES];
obj enemychain[ABSOLUTE_MAX_ENEMY_BULLETS];
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
    auto& settings = GameSettings::getInstance();
    printf("Current wave: %d, Total waves: %d\n", current_wave, TOTAL_WAVES);
    
    // Only spawn regular enemies if we haven't gone through all waves
    if (current_wave < TOTAL_WAVES) {
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
    // Only try to spawn battlestar after all waves are truly complete
    else if (current_wave == TOTAL_WAVES && !battlestar.isalive && !gameCompleted) {
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
}


bool isWaveDefeated() {
    if (current_wave == TOTAL_WAVES) {
        return !battlestar.isalive;
    }
    
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

void updateEnemyWaves() {
    if (gameCompleted) {
        return;
    }

    if (!wave_in_progress) {
        initializeWave();
    }
    else if (isWaveDefeated()) {
        if (current_wave < TOTAL_WAVES) {
            printf("Wave %d completed! Enemies defeated: %d\n", 
                   current_wave + 1, enemies_defeated);
            current_wave++;
            wave_in_progress = false;
            enemies_defeated += ENEMIES_PER_WAVE;
        }
        else if (current_wave == TOTAL_WAVES && !battlestar.isalive) {
            printf("Game completed! All waves and battlestar defeated!\n");
            gameCompleted = true;
        }
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

    player.speed_multiplier = 1.0f;
    player.current_speed = BASE_PLAYER_SPEED;
    
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
    auto& settings = GameSettings::getInstance();
    std::srand(std::time(nullptr));
    loadEnemyTextures();

    // Initialize all possible enemy slots
    for (int i = 0; i < ABSOLUTE_MAX_ENEMIES; i++) {
        enemy[i].x = 640 + (i % ENEMIES_PER_WAVE) * 100;
        enemy[i].y = 100 + (i % ENEMIES_PER_WAVE) * 60;
        enemy[i].imgX = 64;
        enemy[i].imgY = 64;
        enemy[i].health = 50;
        enemy[i].hitbox_offset_x = 16;
        enemy[i].hitbox_offset_y = 16;
        enemy[i].hitbox_width = 32;
        enemy[i].hitbox_height = 32;
        enemy[i].deathPoints = 10;
        enemy[i].initialY = enemy[i].y;
        enemy[i].wave_number = i / ENEMIES_PER_WAVE;
        enemy[i].frequency = settings.getEnemyFrequency() + (rand() % 10) * 0.01f;
        enemy[i].amplitude = settings.getEnemyAmplitude() + (rand() % 30);
        enemy[i].phase = rand() % 360;
        enemy[i].isalive = 0;
        
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

void loadEnemyChain() {
    // Initialize all possible enemy bullet slots
    for(int i = 0; i < ABSOLUTE_MAX_ENEMY_BULLETS; i++) {
        enemychain[i].x = 0;
        enemychain[i].y = 0;
        enemychain[i].imgX = 8;
        enemychain[i].imgY = 8;
        enemychain[i].hitbox_offset_x = 0;
        enemychain[i].hitbox_offset_y = 0;
        enemychain[i].hitbox_width = 8;
        enemychain[i].hitbox_height = 8;
        enemychain[i].texture_pointer = pvr_mem_malloc(8 * 8 * 2);
        if(!enemychain[i].texture_pointer) {
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


void loadEnemyBattlestar() {
    printf("Initializing battlestar...\n");
    
    battlestar.x = 640;
    battlestar.y = (480/2) - 64;
    battlestar.imgX = 128;
    battlestar.imgY = 128;
    battlestar.health = 1000;
    battlestar.hitbox_offset_x = 8;
    battlestar.hitbox_offset_y = 9;
    battlestar.hitbox_width = (120-8);
    battlestar.hitbox_height = (120-9);
    battlestar.damage_state = 0;
    battlestar.isalive = 0;
    battlestar.deathPoints = 100;

    loadEnemyBattlestarTextures();

    bool texturesLoaded = true;
    for (int i = 0; i < MAX_DAMAGE_STATES; i++) {
        if (!g_enemybattlestar_textures[i]) {
            printf("Failed to load battlestar texture %d\n", i);
            texturesLoaded = false;
            break;
        }
        battlestar.damage_textures[i] = g_enemybattlestar_textures[i];
    }

    if (texturesLoaded) {
        battlestar.texture_pointer = battlestar.damage_textures[0];
        printf("Battlestar initialized successfully\n");
    } else {
        printf("Failed to initialize battlestar textures\n");
    }
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


void resetGameState() {
    // Reset game progression variables
    current_wave = 0;
    enemies_defeated = 0;
    wave_in_progress = false;
    gameCompleted = false;

    // Reset player state
    player.health = 1000;
    player.damage_state = 0;
    player.x = 320;
    player.y = 240;
    player.isalive = 1;
    player.speed_multiplier = 1.0f;
    player.current_speed = BASE_PLAYER_SPEED;

    // Reset battlestar
    battlestar.isalive = 0;
    battlestar.health = 1000;
    battlestar.damage_state = 0;

    // Reset all enemies
    for (int i = 0; i < ABSOLUTE_MAX_ENEMIES; i++) {
        enemy[i].isalive = 0;
    }

    // Reset all bullets
    for (int i = 0; i < MAX_NUM_BULLETS; i++) {
        chain[i].isalive = 0;
    }
    for (int i = 0; i < ABSOLUTE_MAX_ENEMY_BULLETS; i++) {
        enemychain[i].isalive = 0;
    }
}

void cleanupTextures() {
    // Free background textures
    if (background.texture_pointer) {
        pvr_mem_free(background.texture_pointer);
        background.texture_pointer = nullptr;
    }
    if (starfield.texture_pointer) {
        pvr_mem_free(starfield.texture_pointer);
        starfield.texture_pointer = nullptr;
    }

    // Free bullet textures
    for (int i = 0; i < MAX_NUM_BULLETS; i++) {
        if (chain[i].texture_pointer) {
            pvr_mem_free(chain[i].texture_pointer);
            chain[i].texture_pointer = nullptr;
        }
    }
    for (int i = 0; i < ABSOLUTE_MAX_ENEMY_BULLETS; i++) {
        if (enemychain[i].texture_pointer) {
            pvr_mem_free(enemychain[i].texture_pointer);
            enemychain[i].texture_pointer = nullptr;
        }
    }
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