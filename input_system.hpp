#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include <kos.h>
#include "loadobj.hpp"
#include "game_constants.hpp"

class InputSystem {
public:
    InputSystem();
    
    void handlePlayerInput(obj& player);
    bool shouldShoot() const;
    
private:
    maple_device_t* controller;
    cont_state_t* state;
    bool shootPressed;
    
    void updateControllerState();
    void handleMovement(obj& player, float dx, float dy);
    void handleSpeedBoost(obj& player);
};

#endif // INPUT_SYSTEM_HPP