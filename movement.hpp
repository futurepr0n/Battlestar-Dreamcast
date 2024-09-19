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
int checkCollision(obj blt);

#ifdef __cplusplus
}
#endif
#endif