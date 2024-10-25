/* png example for KOS 1.1.x
 * hacked up to shit by futurepr0n
 */

#include <kos.h>
#include <iostream>
#include <png/png.h>
//extern "C"{
    #include <zlib/zlib.h>
//}
#include <mp3/sndserver.h>
#include <modplug/stdafx.h>
#include <modplug/sndfile.h>

#include "loadobj.hpp"
#include "initstorysplash.hpp"
#include "blitobj.hpp"
#include "movement.hpp"
#include "audio_system.h"
#include "renderer.hpp"
#include "game_utils.hpp"
#include "menu_system.hpp"
#include "name_selection.hpp" 
#include "game_state.hpp"
#include "game_constants.hpp"
#include "endgamesplash.hpp"

#include <cmath>
#include <chrono>



int mybullets = 0;


int main(int argc, char **argv) {
    int done = 0;

    /* controller initialization */
    maple_device_t *cont;
    cont_state_t *state;
    /* -------------- */

    /* init kos  */
    pvr_init_defaults();
    load_font_texture();
    /* init splash screen */
    initSplash();

    std::string playerName = showMainMenu();
    GameState::getInstance().setPlayerName(playerName);
    GameState::getInstance().resetScore();

    std::cout << "Debug - Player name set to: " << GameState::getInstance().getPlayerName() << std::endl;

    initStorySplash();

    // Load enemy textures before loading character data
    loadEnemyTextures();
    loadPlayerTextures();
    loadEnemyBattlestarTextures();
    loadCharacterData(); // loadobj.cpp

    if (!g_audioSystem.initialize("/rd/mrdeath.xm")) {
        printf("Failed to initialize audio system\n");
        return 1;
    }

    while(!gameCompleted) {
    draw_scene();
    cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

    if(cont) {
        state = (cont_state_t *)maple_dev_status(cont);

        if(state) {
            bool moving_diagonal = false;
            float dx = 0.0f;
            float dy = 0.0f;
            float current_speed = BASE_PLAYER_SPEED * player.speed_multiplier;

            // Handle joystick input with deadzone and normalized movement
            if (abs(state->joyx) > 10 || abs(state->joyy) > 10) {
                // Convert joystick values to -1.0 to 1.0 range
                float joy_x = state->joyx / 128.0f;
                float joy_y = state->joyy / 128.0f;

                // Apply deadzone
                if (fabsf(joy_x) < 0.1f) joy_x = 0.0f;
                if (fabsf(joy_y) < 0.1f) joy_y = 0.0f;

                // Check if moving diagonally
                if (joy_x != 0.0f && joy_y != 0.0f) {
                    moving_diagonal = true;
                }

                // Calculate movement
                dx = joy_x * current_speed;
                dy = joy_y * current_speed;
            }
            // Handle D-pad input
            else {
                if (state->buttons & CONT_DPAD_LEFT)  dx = -current_speed;
                if (state->buttons & CONT_DPAD_RIGHT) dx = current_speed;
                if (state->buttons & CONT_DPAD_UP)    dy = -current_speed;
                if (state->buttons & CONT_DPAD_DOWN)  dy = current_speed;

                // Check if moving diagonally
                if (dx != 0.0f && dy != 0.0f) {
                    moving_diagonal = true;
                }
            }

            // Apply diagonal movement compensation
            if (moving_diagonal) {
                dx *= DIAGONAL_COMPENSATION;
                dy *= DIAGONAL_COMPENSATION;
            }

            // Update position with boundary checking
            float new_x = player.x + dx;
            float new_y = player.y + dy;

            // Screen boundary checking
            if (new_x >= player.imgX/2 && new_x <= 640 - player.imgX/2) {
                player.x = new_x;
            }
            if (new_y >= player.imgY/2 && new_y <= 480 - player.imgY/2) {
                player.y = new_y;
            }

            // Boost handling (example using B button)
            if (state->buttons & CONT_A) {
                player.speed_multiplier = BOOST_MULTIPLIER;
            } else {
                player.speed_multiplier = 1.0f;
            }

            if(state->buttons & CONT_B)
                shootChain();
        }
    }
        g_audioSystem.update();
        // Update game state
        updateEnemyWaves();

        timer_spin_sleep(10);
        
    }

    g_audioSystem.cleanup();

    // Unload enemy textures
    unloadEnemyTextures();
    unloadPlayerTextures();
    unloadEnemyBattlestarTextures();

    // Show victory screen
    showVictoryScreen();

    return 0;
}

