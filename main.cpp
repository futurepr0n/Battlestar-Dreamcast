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

    /* init splash screen */
    initSplash();
    initStorySplash();
    loadCharacterData(); // loadobj.cpp

    if (!g_audioSystem.initialize("/rd/mrdeath.xm")) {
        printf("Failed to initialize audio system\n");
        return 1;
    }

    while(1) {

        draw_scene();
        /* Check key status */
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

        if(cont) {
            state = (cont_state_t *)maple_dev_status(cont);

            if(state && state->buttons & CONT_START)
                break;
            
            if(state && state->buttons & CONT_DPAD_LEFT)
                player.x--;
            
            if(state && state->buttons & CONT_DPAD_RIGHT)
                player.x++;

            if(state && state->buttons & CONT_DPAD_UP)
                player.y--;
            
            if(state && state->buttons & CONT_DPAD_DOWN)
                player.y++;


            // Use joystick for movement
            if (state) {
                // Check the X axis of the main joystick
                if (state->joyx < -10) { // Threshold to avoid sensitivity issues
                    player.x--;  // Move left
                } else if (state->joyx > 10) {
                    player.x++;  // Move right
                }

                // Check the Y axis of the main joystick
                if (state->joyy < -10) {
                    player.y--;  // Move up
                } else if (state->joyy > 10) {
                    player.y++;  // Move down
                }
            }

            if(state && state->buttons & CONT_B)
                shootChain();

        }
        g_audioSystem.update();

        timer_spin_sleep(10);
        
    }

    g_audioSystem.cleanup();

    return 0;
}

