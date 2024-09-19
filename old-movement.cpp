#include "movement.hpp"
#include "loadobj.hpp"
#include "blitobj.hpp"

int bullets = 0;
int p = 0;

int checkCollision(obj blt){
	int c_ctr;
	if(blt.isalive == 1){
		//blt.x = blt.x + (8 * 1.5F);
		for(c_ctr = 0; c_ctr < MAX_NUM_ENEMIES; c_ctr++){	
			if((blt.x >= enemy[c_ctr].x && blt.y < enemy[c_ctr].y + 20) && (blt.x < enemy[c_ctr].x + 42 && blt.y >= enemy[c_ctr].y)){
				enemy[c_ctr].health = enemy[c_ctr].health - 10;
				if(enemy[c_ctr].health < 0){
					enemy[c_ctr].isalive = 0;
				}	
				//points = points + 10;
				blt.isalive = 0;
			}
		}
		if((blt.x >= battlestar.x && blt.y < battlestar.y + 76) && (blt.x < battlestar.x + 220 && blt.y >= battlestar.y)){
				battlestar.health = battlestar.health - 10;
				if(battlestar.health < 0){
					battlestar.isalive = 0;
					//points = points + 1000;
				}	
				
				blt.isalive = 0;
			}
	}
	return(blt.isalive);
}


void moveStuff(){
    //scrollBackground();
	//scrollStarfield();
    if(battlestar.x >= (640-230)){
		battlestar.x = battlestar.x - 5;
	}

            for(p = 0; p < MAX_NUM_BULLETS; p++){
			    chain[p].isalive = checkCollision(chain[p]);
			    if(chain[p].isalive == 1){
       			    chain[p].x = chain[p].x + 10  * 1.5F;      			
				    blitObj(chain[p]);					
			    }else{
				    chain[p].isalive = 0;
				    chain[p].pctr = 0;
			    }			
			    if(chain[p].x > 640 + chain[p].imgX){
					chain[p].isalive = 0;
					chain[p].pctr = 0;
			    }
		    }     

			for(p = 0; p < MAX_NUM_ENEMY_BULLETS; p++){
			    enemychain[p].isalive = checkCollision(enemychain[p]);
			    if(enemychain[p].isalive == 1){
       			    enemychain[p].x = enemychain[p].x - 10 * 1.5F;      			
				    blitObj(enemychain[p]);					
			    }else{
				    enemychain[p].isalive = 0;
				    enemychain[p].pctr = 0;
			    }			
			    if(enemychain[p].x < 0){
					enemychain[p].isalive = 0;
					enemychain[p].pctr = 0;
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

