// loadobj.h: headers for loadobj
//
// 
//
//////////////////////////////////////////////////////////////////////
#ifndef _LOADOBJ_HPP_
#define _LOADOBJ_HPP_

#include <kos.h>
#include "game_constants.hpp"
#ifdef __cplusplus
extern "C" {
#endif


//int bullets = 0;
// int points = 0;
// int hScore = 200;
// char playerScore[5];
// char highScore[5];

typedef struct{
	float x;
	float y;
	float prev_x;  // Add this line
    float prev_y;  // Add this line
	float imgX;
	float imgY;
	int hitbox_offset_x;
	int hitbox_offset_y;
	int hitbox_width;
	int hitbox_height;
    int health;
	float frequency;
	float amplitude;
	float phase;
	float initialX;
	float initialY;
	int deathPoints;  // New field for score points
	int wave_number;  // New field to track which wave the enemy belongs to
	float speed_multiplier;  // For power-ups and speed modifications
    float current_speed;
	//int dir;
	void *img_data;
    pvr_ptr_t texture_pointer;
	pvr_ptr_t damage_textures[MAX_DAMAGE_STATES];  // Array of texture pointers for different damage states
    int damage_state;  // Current damage state
	int isalive;	
	int pctr;
}obj;

// Arrays sized to maximum possible values
extern obj player;
extern obj enemy[ABSOLUTE_MAX_ENEMIES];
extern obj chain[MAX_NUM_BULLETS];
extern obj enemychain[ABSOLUTE_MAX_ENEMY_BULLETS];
extern obj background;
extern obj starfield;
extern obj battlestar;

extern int current_wave;
extern int enemies_defeated;
extern bool wave_in_progress;
extern bool gameCompleted;  // Add this to track game state


//obj background;
//obj starfield;
//obj player;
//obj sprite;
//obj enemy[MAX_NUM_ENEMIES];
//obj chain[MAX_NUM_BULLETS];
//obj enemychain[MAX_NUM_ENEMY_BULLETS];
//obj battlestar;
//obj hs_bar;
//obj status_bar;


//  Function prototypes for public functions
//    

//private functions
	void initializeWave();
	bool isWaveDefeated();
	void updateEnemyWaves();

    void loadPlayer();
    void loadEnemies();
    void loadChain();
	void loadEnemyChain();
    void loadCharacterData();
	void loadBackground();
	void loadEnemyBattlestar();
	void loadStarfield();
	void loadPlayerTextures();
	void unloadPlayerTextures();
	void loadEnemyTextures();
	void unloadEnemyTextures();
	void loadEnemyBattlestarTextures();
	void unloadEnemyBattlestarTextures();
	void resetGameState();
	void cleanupTextures();
	//void loadStatusBar();
	//void loadHsBar();

#ifdef __cplusplus
}
#endif
#endif