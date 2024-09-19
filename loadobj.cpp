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

    player.texture_pointer = pvr_mem_malloc(64 * 64 * 2);
    if(!player.texture_pointer){
		printf("Player image failed to load...");
	}
    png_to_texture("/rd/vipersm2.png", player.texture_pointer, PNG_FULL_ALPHA);

    player.health = 1000;

}

void loadChain(){
	int i;	
	for(i = 0; i < MAX_NUM_BULLETS; i++){
		chain[i].x = 0;
		chain[i].y = 0;
		chain[i].imgX = 8;
		chain[i].imgY = 8;
		chain[i].hitbox_offset_x = 0;
		chain[i].hitbox_offset_y = 3;
		chain[i].hitbox_width = (8-0);
		chain[i].hitbox_height = (8-5);
		chain[i].texture_pointer = pvr_mem_malloc(8 * 8 * 2);
    	if(!chain[i].texture_pointer){
			printf("chain image failed to load...");
		}
    	png_to_texture("/rd/chain.png", chain[i].texture_pointer, PNG_FULL_ALPHA);
		chain[i].isalive = 0;
		chain[i].pctr = 0;
	}
}

void loadEnemies() {
    std::srand(std::time(nullptr)); // Seed the random number generator

    for (int i = 0; i < MAX_NUM_ENEMIES; i++) {
        enemy[i].x = 480 + (rand() % 10000); // Random X start position
        enemy[i].y = 1 + (rand() % (480 - 64)); // Random Y position within screen height
        enemy[i].imgX = 64;
        enemy[i].imgY = 64;
        enemy[i].health = 50;
		enemy[i].hitbox_offset_x = 11;
		enemy[i].hitbox_offset_y = 22;
		enemy[i].hitbox_width = (52-11);
		enemy[i].hitbox_height = (41-22);

		  // Set initial Y position within the screen height
        enemy[i].initialY = 1 + (rand() % (480 - 64)); // Adjust based on screen height and enemy height
        enemy[i].y = enemy[i].initialY; // Set current Y to initial Y
        
        // Allocate memory for the texture
        enemy[i].texture_pointer = pvr_mem_malloc(64 * 64 * 2);
        if (!enemy[i].texture_pointer) {
            printf("Chain image failed to load...");
        }

        png_to_texture("/rd/raidersm.png", enemy[i].texture_pointer, PNG_FULL_ALPHA);

        // Set random values for vertical movement
        enemy[i].frequency = 0.02f + (rand() % 10) * 0.01f; // Random frequency between 0.02 and 0.12
        enemy[i].amplitude = 10.0f + (rand() % 30); // Random amplitude between 10 and 40
        enemy[i].phase = rand() % 360; // Random phase shift in degrees

        enemy[i].isalive = 1;
        enemy[i].pctr = 0;
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
		enemychain[i].hitbox_offset_y = 3;
		enemychain[i].hitbox_width = (8-0);
		enemychain[i].hitbox_height = (8-5);
		enemychain[i].texture_pointer = pvr_mem_malloc(8 * 8 * 2);
		if(!enemychain[i].texture_pointer){
			printf("enemychain image failed to load...");
		}
		png_to_texture("/rd/enemychain.png", enemychain[i].texture_pointer, PNG_FULL_ALPHA);
		enemychain[i].isalive = 0;
		enemychain[i].pctr = 0;
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
	battlestar.hitbox_width = (52-8);
	battlestar.hitbox_height = (43-9);
	battlestar.texture_pointer = pvr_mem_malloc(128 * 128 * 2);
		if(!battlestar.texture_pointer){
			printf("battlestar.img failed to load..");
		}
	png_to_texture("/rd/battlestar.png", battlestar.texture_pointer, PNG_FULL_ALPHA);
	battlestar.isalive = 1;
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