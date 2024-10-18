// loadobj.h: headers for loadobj
//
// 
//
//////////////////////////////////////////////////////////////////////
#ifndef _LOADOBJ_HPP_
#define _LOADOBJ_HPP_

#include <kos.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NUM_BULLETS 3
#define MAX_NUM_ENEMY_BULLETS 3
#define MAX_NUM_ENEMIES 100
#define MAX_DAMAGE_STATES 6  // 5 damage states + 1 original state


//int bullets = 0;
// int points = 0;
// int hScore = 200;
// char playerScore[5];
// char highScore[5];

typedef struct{
	int x;
	int y;
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

	//int dir;
	void *img_data;
    pvr_ptr_t texture_pointer;
	pvr_ptr_t damage_textures[MAX_DAMAGE_STATES];  // Array of texture pointers for different damage states
    int damage_state;  // Current damage state
	int isalive;	
	int pctr;
}obj;

extern obj player;
extern obj enemy[MAX_NUM_ENEMIES];
extern obj chain[MAX_NUM_BULLETS];
extern obj enemychain[MAX_NUM_ENEMY_BULLETS];
//obj prs_circle;
//obj spl_background;
extern obj background;
extern obj starfield;
extern obj battlestar;



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
	//void loadStatusBar();
	//void loadHsBar();

#ifdef __cplusplus
}
#endif
#endif