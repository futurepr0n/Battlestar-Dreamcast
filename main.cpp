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

#include <cmath>
#include <chrono>

std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();

float getDeltaTime() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    duration<float> delta = now - lastFrameTime;
    lastFrameTime = now;
    return delta.count(); // Return time in seconds
}

int mybullets = 0;

/** MikMod Stuff **/
uint16 sound_buffer[65536] = {0};
CSoundFile *soundfile;

void *mod_callback(snd_stream_hnd_t hnd, int len, int * actual) {
    int res;

    res = soundfile->Read(sound_buffer, len) * 4/*samplesize*/;

    //printf("res: %i, len: %i\n",res,len);
    if(res < len) {
        soundfile->SetCurrentPos(0);
        soundfile->Read(&sound_buffer[res], len - res);
    }

    *actual = len;

    return sound_buffer;
}

void draw_background(void) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565, 512, 512, background.texture_pointer, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;

    vert.x = 0.0f;
    vert.y = 0.0f;
    vert.z = 1.0f;
    vert.u = 0.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    vert.x = 640.0f;
    vert.y = 0.0f;
    vert.z = 1.0f;
    vert.u = 1.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    vert.x = 1.0f;
    vert.y = 480.0f;
    vert.z = 1.0f;
    vert.u = 0.0f;
    vert.v = 1.0f;
    pvr_prim(&vert, sizeof(vert));

    vert.x = 640.0f;
    vert.y = 480.0f;
    vert.z = 1.0f;
    vert.u = 1.0f;
    vert.v = 1.0f;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
}

void draw_ship() {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    // Update texture size to 64x32 (next power of 2 for 46x24)
    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 64, 64, player.texture_pointer, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;

    float half_width = 64.0f / 2;
    float half_height = 64.0f / 2;

    // Calculate UV coordinates
    float u_max = 64.0f / 64.0f;  // 46 / next power of 2
    float v_max = 64.0f / 64.0f;  // 24 / next power of 2

    // Top-left vertex
    vert.x = player.x - half_width;
    vert.y = player.y - half_height;
    vert.z = 1.0f;
    vert.u = 0.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Top-right vertex
    vert.x = player.x + half_width;
    vert.y = player.y - half_height;
    vert.u = u_max;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-left vertex
    vert.x = player.x - half_width;
    vert.y = player.y + half_height;
    vert.u = 0.0f;
    vert.v = v_max;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-right vertex
    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.x = player.x + half_width;
    vert.y = player.y + half_height;
    vert.u = u_max;
    vert.v = v_max;
    pvr_prim(&vert, sizeof(vert));
}

/* draw the starfield */
void draw_starfield(float x1, float y1, float z1, float a, float r, float g, float b, int c, float xs, float ys) {
    pvr_vertex_t    vert;
    int             ix, iy;
    float           u1, v1, u2, v2;

    ix = (c % 32) * 8;
    iy = (c / 32) * 16;
    u1 = (ix + 0.5f) * 1.0f / 256.0f;
    v1 = (iy + 0.5f) * 1.0f / 256.0f;
    u2 = (ix + 7.5f) * 1.0f / 256.0f;
    v2 = (iy + 15.5f) * 1.0f / 256.0f;

    vert.flags = PVR_CMD_VERTEX;
    vert.x = x1;
    vert.y = y1 + 16.0f * ys;
    vert.z = z1;
    vert.u = u1;
    vert.v = v2;
    vert.argb = PVR_PACK_COLOR(a, r, g, b);
    vert.oargb = 0;
    pvr_prim(&vert, sizeof(vert));

    vert.x = x1;
    vert.y = y1;
    vert.u = u1;
    vert.v = v1;
    pvr_prim(&vert, sizeof(vert));

    vert.x = x1 + 8.0f * xs;
    vert.y = y1 + 16.0f * ys;
    vert.u = u2;
    vert.v = v2;
    pvr_prim(&vert, sizeof(vert));

    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.x = x1 + 8.0f * xs;
    vert.y = y1;
    vert.u = u2;
    vert.v = v1;
    pvr_prim(&vert, sizeof(vert));
} 


void draw_scrolling_background(float x_offset) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 512, 512, starfield.texture_pointer, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    // Draw first copy of the background
    vert.argb = PVR_PACK_COLOR(1, 1, 1, 1);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;

    // Top-left vertex
    vert.x = x_offset;
    vert.y = 0;
    vert.z = 3;
    vert.u = 0.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Top-right vertex
    vert.x = x_offset + 640.0f;
    vert.y = 0;
    vert.u = 1.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-left vertex
    vert.x = x_offset;
    vert.y = 480.0f;
    vert.u = 0.0f;
    vert.v = 1.0f;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-right vertex
    vert.x = x_offset + 640.0f;
    vert.y = 480.0f;
    vert.u = 1.0f;
    vert.v = 1.0f;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));

    // Draw second copy of the background
    vert.flags = PVR_CMD_VERTEX;

    // Top-left vertex
    vert.x = x_offset + 640.0f;
    vert.y = 0;
    vert.z = 3;
    vert.u = 0.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Top-right vertex
    vert.x = x_offset + 1280.0f;
    vert.y = 0;
    vert.u = 1.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-left vertex
    vert.x = x_offset + 640.0f;
    vert.y = 480.0f;
    vert.u = 0.0f;
    vert.v = 1.0f;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-right vertex
    vert.x = x_offset + 1280.0f;
    vert.y = 480.0f;
    vert.u = 1.0f;
    vert.v = 1.0f;
    vert.flags = PVR_CMD_VERTEX_EOL;
    pvr_prim(&vert, sizeof(vert));
}


/* Draw the scene for the gameplay*/
void draw_scene(void) {
    static float x_offset = 0;

    pvr_wait_ready();
    pvr_scene_begin();

    pvr_list_begin(PVR_LIST_OP_POLY);
    draw_background();
    pvr_list_finish();

    pvr_list_begin(PVR_LIST_TR_POLY);
    draw_scrolling_background(x_offset);
    moveStuff();
    float deltaTime = getDeltaTime(); // Implement this to calculate time between frames
    blitObj(player);
    blitEnemies(deltaTime);
    blitObj(battlestar);
    pvr_list_finish();
    pvr_scene_finish();


    x_offset -= 1.0f;
    if (x_offset <= -640.0f) {
        x_offset += 640.0f;
    }
}



int main(int argc, char **argv) {
    int done = 0;

    /* controller initialization */
    maple_device_t *cont;
    cont_state_t *state;
    /* -------------- */

    /*  MikMod Stuff    */
    uint8 *mod_buffer;
    uint32 hnd;
    char filename[] = "/rd/mrdeath.xm";

    /********************/

    /* init kos  */
    pvr_init_defaults();

    /* init splash screen */
    initSplash();
    initStorySplash();
    loadCharacterData(); // loadobj.cpp

    printf("modplug_test beginning\n");

    snd_stream_init();

    hnd = fs_open(filename, O_RDONLY);

    if(!hnd) {
        printf("Error reading %s\n", filename);
        return 0;
    }

    printf("Filesize: %i\n", fs_total(hnd));
    mod_buffer = (uint8 *)malloc(fs_total(hnd));

    if(!mod_buffer) {
        printf("Not enough memory\n");
        return 0;
    }

    printf("Memory allocated\n");

    if((size_t)fs_read(hnd, mod_buffer, fs_total(hnd)) != fs_total(hnd)) {
        printf("Read error\n");
        free(mod_buffer);
        return 0;
    }

    printf("File read\n");

    soundfile = new CSoundFile;

    if(!soundfile) {
        printf("Not enough memory\n");
        free(mod_buffer);
        return 0;
    }

    printf("CSoundFile created\n");

    if(!soundfile->Create(mod_buffer, fs_total(hnd))) {
        printf("Mod not loaded\n");
        free(mod_buffer);
        delete soundfile;
        return 0;
    }

    printf("Mod loaded\n");
    soundfile->SetWaveConfig(44100, 16, 2);
    printf("Type: %li\n", soundfile->GetType());
    printf("Title: %s\n", soundfile->GetTitle());

    /*fs_close(hnd);
    free(mod_buffer);*/

    snd_stream_hnd_t shnd = snd_stream_alloc(mod_callback, SND_STREAM_BUFFER_MAX);
    snd_stream_start(shnd, 44100, 1);

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
        snd_stream_poll(shnd);

        timer_spin_sleep(10);
        
    }

    delete soundfile;

    snd_stream_destroy(shnd);

    snd_stream_shutdown();

    return 0;
}

