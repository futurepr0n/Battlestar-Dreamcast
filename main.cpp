/* png example for KOS 1.1.x
 * hacked up to shit by futurepr0n
 */

#include <kos.h>
#include <iostream>
#include <png/png.h>
#include <zlib/zlib.h>
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
#include "input_system.hpp"
#include "wave_manager.hpp"
#include "bullet_manager.hpp"
#include "game_engine.hpp"

#include <cmath>
#include <chrono>

int mybullets = 0;

int main(int argc, char **argv) {
    /* init kos  */
    pvr_init_defaults();

    while(1) {  // Main game loop

        // Initialize unified game engine
        GameEngine gameEngine;

        // Font loading is now handled by FontManager in GameEngine
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

        // Initialize the game engine with all systems
        if (!gameEngine.initialize()) {
            printf("Failed to initialize game engine\n");
            return 1;
        }

        // Initialize essential systems
        InputSystem inputSystem;
        
        // Initialize the first wave of enemies
        initializeWave();
        
        while(!gameCompleted) {
            // Essential system updates (lightweight) 
            inputSystem.handlePlayerInput(player);
            if (inputSystem.shouldShoot()) {
                shootChain(); // Use original shooting function
            }
            
            g_audioSystem.update();
            updateEnemyWaves(); // Check wave completion and advance to next wave
            
            draw_scene();
            timer_spin_sleep(10);
        }

        // Print final performance report
        gameEngine.printPerformanceReport();
        gameEngine.cleanup();

        // Unload textures
        //cleanupTextures();
        unloadEnemyTextures();
        unloadPlayerTextures();
        unloadEnemyBattlestarTextures();

        showVictoryScreen();
    }

    return 0;
}