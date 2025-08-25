#include "input_system.hpp"
#include <cmath>

InputSystem::InputSystem() : controller(nullptr), state(nullptr), shootPressed(false) {
}

void InputSystem::updateControllerState() {
    controller = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    state = controller ? (cont_state_t *)maple_dev_status(controller) : nullptr;
}

void InputSystem::handlePlayerInput(obj& player) {
    updateControllerState();
    
    if (!state) return;
    
    bool moving_diagonal = false;
    float dx = 0.0f;
    float dy = 0.0f;
    float current_speed = BASE_PLAYER_SPEED * player.speed_multiplier;

    // Handle joystick input with deadzone and normalized movement
    if (abs(state->joyx) > 10 || abs(state->joyy) > 10) {
        float joy_x = state->joyx / 128.0f;
        float joy_y = state->joyy / 128.0f;

        if (fabsf(joy_x) < 0.1f) joy_x = 0.0f;
        if (fabsf(joy_y) < 0.1f) joy_y = 0.0f;

        if (joy_x != 0.0f && joy_y != 0.0f) {
            moving_diagonal = true;
        }

        dx = joy_x * current_speed;
        dy = joy_y * current_speed;
    } else {
        // Handle D-pad input
        if (state->buttons & CONT_DPAD_LEFT)  dx = -current_speed;
        if (state->buttons & CONT_DPAD_RIGHT) dx = current_speed;
        if (state->buttons & CONT_DPAD_UP)    dy = -current_speed;
        if (state->buttons & CONT_DPAD_DOWN)  dy = current_speed;

        if (dx != 0.0f && dy != 0.0f) {
            moving_diagonal = true;
        }
    }

    // Apply diagonal compensation
    if (moving_diagonal) {
        dx *= DIAGONAL_COMPENSATION;
        dy *= DIAGONAL_COMPENSATION;
    }

    handleMovement(player, dx, dy);
    handleSpeedBoost(player);
    
    // Update shoot button state
    shootPressed = (state->buttons & CONT_B) != 0;
}

void InputSystem::handleMovement(obj& player, float dx, float dy) {
    float new_x = player.x + dx;
    float new_y = player.y + dy;

    // Apply boundary checking
    if (new_x >= player.imgX/2 && new_x <= SCREEN_WIDTH - player.imgX/2) {
        player.x = new_x;
    }
    if (new_y >= player.imgY/2 && new_y <= SCREEN_HEIGHT - player.imgY/2) {
        player.y = new_y;
    }
}

void InputSystem::handleSpeedBoost(obj& player) {
    if (!state) return;
    
    if (state->buttons & CONT_A) {
        player.speed_multiplier = BOOST_MULTIPLIER;
    } else {
        player.speed_multiplier = 1.0f;
    }
}

bool InputSystem::shouldShoot() const {
    return shootPressed;
}