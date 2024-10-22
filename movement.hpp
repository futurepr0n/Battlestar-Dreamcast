//movement.h
#ifndef _MOVEMENT_HPP_
#define _MOVEMENT_HPP_

#include "loadobj.hpp"
#include "blitobj.hpp"
#ifdef __cplusplus
extern "C" {
#endif

void shootChain();
//void shootEnenmyChain();
void moveStuff();
int checkCollision(obj* obj1, obj* obj2);  // This is the correct declaration
bool checkBulletCollision(obj* bullet, obj* target);
void shootEnemyChain(int enemyIndex);

#ifdef __cplusplus
}
#endif
#endif