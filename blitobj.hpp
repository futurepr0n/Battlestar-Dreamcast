#ifndef _BLITOBJ_HPP_
#define _BLITOBJ_HPP_

#include "movement.hpp"
#include "loadobj.hpp"

#ifdef __cplusplus
extern "C" {
#endif

//private functions
    void blitObj(const obj& object);
    //void blitBg(obj object);
    void blitEnemies(float deltaTime);
    void shootEnemyChain(int z);

#ifdef __cplusplus
}
#endif
#endif
