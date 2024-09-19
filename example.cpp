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




long bitrateold, bitratenew;

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);

/* font data */
extern char wfont[];


extern "C" {
    int zlib_getlength(char*);
} 

/* textures */
pvr_ptr_t font_tex;
pvr_ptr_t back_tex;
pvr_ptr_t bsg_back_tex;
pvr_ptr_t background_tex;
pvr_ptr_t starfield_tex;
char *data;

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
/** MikMod Stuff **/


/* Stuff from my battlestar*/
//int done = 0;
//int bullets = 0;
//char highScore[5];
//char playerScore[5];
//int points = 0;
//int hScore = 1000;

typedef struct{
	int x;
	int y;
	int imgX;
	int imgY;
	//int dir;
	void *img_data;
    pvr_ptr_t texture_pointer;
	int isalive;	
	int pctr;
}obj;
//obj prs_circle;
//obj spl_background;
//obj background;
//obj starfield;
obj player;
//obj enemy[MAX_NUM_ENEMIES];
//obj chain[MAX_NUM_BULLETS];

void initSplash();
void initStorySplash();
void initStarfield();
void initBackground();

void initShip();


/* init background */
void back_init(void) {
    back_tex = pvr_mem_malloc(512 * 512 * 2);
    png_to_texture("/rd/background.png", back_tex, PNG_NO_ALPHA);
}

/* init background */
void bsg_back_init(void) {
    bsg_back_tex = pvr_mem_malloc(512 * 512 * 2);
    png_to_texture("/rd/bsg-open-title.png", bsg_back_tex, PNG_NO_ALPHA);
}


/* init font */
void font_init(void) {
    int i, x, y, c;
    unsigned short * temp_tex;

    font_tex = pvr_mem_malloc(256 * 256 * 2);
    temp_tex = (unsigned short *)malloc(256 * 128 * 2);

    c = 0;

    for(y = 0; y < 128 ; y += 16)
        for(x = 0; x < 256 ; x += 8) {
            for(i = 0; i < 16; i++) {
                temp_tex[x + (y + i) * 256 + 0] = 0xffff * ((wfont[c + i] & 0x80) >> 7);
                temp_tex[x + (y + i) * 256 + 1] = 0xffff * ((wfont[c + i] & 0x40) >> 6);
                temp_tex[x + (y + i) * 256 + 2] = 0xffff * ((wfont[c + i] & 0x20) >> 5);
                temp_tex[x + (y + i) * 256 + 3] = 0xffff * ((wfont[c + i] & 0x10) >> 4);
                temp_tex[x + (y + i) * 256 + 4] = 0xffff * ((wfont[c + i] & 0x08) >> 3);
                temp_tex[x + (y + i) * 256 + 5] = 0xffff * ((wfont[c + i] & 0x04) >> 2);
                temp_tex[x + (y + i) * 256 + 6] = 0xffff * ((wfont[c + i] & 0x02) >> 1);
                temp_tex[x + (y + i) * 256 + 7] = 0xffff * (wfont[c + i] & 0x01);
            }

            c += 16;
        }

    pvr_txr_load_ex(temp_tex, font_tex, 256, 256, PVR_TXRLOAD_16BPP);
}

void text_init(void) {
    const char* filename = "/rd/text.gz";
    int length = zlib_getlength(const_cast<char*>(filename));
    //int length = zlib_getlength("/rd/text.gz");
    gzFile f;

    data = (char *)malloc(length + 1); // I am not currently freeing it

    f = gzopen("/rd/text.gz", "r");
    gzread(f, data, length);
    data[length] = 0;
    gzclose(f);

    printf("length [%d]\n", length);
}

/* draw background */
void draw_back(void) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565, 512, 512, back_tex, PVR_FILTER_BILINEAR);
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

/* draw the title screen background */
void draw_splash_back(void) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565, 512, 512, bsg_back_tex, PVR_FILTER_BILINEAR);
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

void draw_background(void) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565, 512, 512, background_tex, PVR_FILTER_BILINEAR);
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

void draw_ship(obj Obj) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    // Update texture size to 64x32 (next power of 2 for 46x24)
    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 64, 64, Obj.texture_pointer, PVR_FILTER_BILINEAR);
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
    vert.x = Obj.x - half_width;
    vert.y = Obj.y - half_height;
    vert.z = 1.0f;
    vert.u = 0.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Top-right vertex
    vert.x = Obj.x + half_width;
    vert.y = Obj.y - half_height;
    vert.u = u_max;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-left vertex
    vert.x = Obj.x - half_width;
    vert.y = Obj.y + half_height;
    vert.u = 0.0f;
    vert.v = v_max;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-right vertex
    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.x = Obj.x + half_width;
    vert.y = Obj.y + half_height;
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

/* draw one character */
void draw_char(float x1, float y1, float z1, float a, float r, float g, float b, int c, float xs, float ys) {
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

/* draw a string */
void draw_string(float x, float y, float z, float a, float r, float g, float b, char *str, float xs, float ys) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    float orig_x = x;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 256, 256, font_tex, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    while(*str) {
        if(*str == '\n') {
            x = orig_x;
            y += 40;
            str++;
            continue;
        }

        draw_char(x, y, z, a, r, g, b, *str++, xs, ys);
        x += 8 * xs;
    }
}

/* base y coordinate */
int y = 0;

/* draw one frame */
void draw_frame(void) {
    pvr_wait_ready();
    pvr_scene_begin();

    pvr_list_begin(PVR_LIST_OP_POLY);
    draw_back();
    pvr_list_finish();

    pvr_list_begin(PVR_LIST_TR_POLY);

    /* 1720 and 480 are magic numbers directly related to the text scrolling
     * 1720 is enough room for the whole text to scroll from the bottom of
     * the screen to off the top.  31 lines * 40 pixels + 480 pixel high screen
     * 480 is the height of the screen (starts the text at the bottom)
     */
    draw_string(0, y % 1720 + 440, 3, 1, 1, 1, 1, data, 2, 2);

    pvr_list_finish();
    pvr_scene_finish();

    y--;
}

/* draw spash frame */
void draw_splash_frame(void) {
    pvr_wait_ready();
    pvr_scene_begin();

    pvr_list_begin(PVR_LIST_OP_POLY);
    draw_splash_back();
    pvr_list_finish();

    pvr_list_begin(PVR_LIST_TR_POLY);


    pvr_list_finish();
    pvr_scene_finish();
}

void draw_scrolling_background(float x_offset) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 512, 512, starfield_tex, PVR_FILTER_BILINEAR);
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
    draw_ship(player); // Uncommented and moved inside the translucent polygon list
    pvr_list_finish();

    pvr_scene_finish();

    x_offset -= 1.0f;
    if (x_offset <= -640.0f) {
        x_offset += 640.0f;
    }
}

int main(int argc, char **argv) {
    int done = 0;

    /* from mp3test.c */
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

    initBackground();
   	initStarfield();
    initShip();
    

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

        }
        snd_stream_poll(shnd);

        timer_spin_sleep(10);
        
    }

    delete soundfile;

    snd_stream_destroy(shnd);

    snd_stream_shutdown();

    return 0;
}


void initSplash(){
    int done = 0;
	 /* init background */
    bsg_back_init();

    while(!done) {
        MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)

        if(st->buttons & CONT_START)
            done = 1;

        MAPLE_FOREACH_END()

        draw_splash_frame();
    }

    return;

}

void initStorySplash(){
    int done = 0;
	 /* init background */
     /* init font */
    font_init();
    
    /* init background */
    back_init();

    /* init text */
    text_init();

    /* init soundstream and mp3 stuff (from mp3test) */
    snd_stream_init();
    mp3_init();

    mp3_start("/rd/theme.mp3", 0);

    /* keep drawing frames until start is pressed */
    while(!done) {
        MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)

        if(st->buttons & CONT_START)
            done = 1;

        MAPLE_FOREACH_END()

        draw_frame();
    }

    /* Clean up the sound stuff on exit */
    mp3_stop();
    //mp3_shutdown();
    //snd_stream_shutdown();

    return;

}

void initBackground(){
    background_tex = pvr_mem_malloc(512 * 512 * 2);
    png_to_texture("/rd/space_bg1.png", background_tex, PNG_NO_ALPHA);


}


void initStarfield(){
    starfield_tex = pvr_mem_malloc(512 * 512 * 2);
    png_to_texture("/rd/starfield_one1.png", starfield_tex, PNG_MASK_ALPHA);
}

void initShip(){
    // Allocate memory for 64x32 texture (next power of 2 for 46x24)
    player.texture_pointer = pvr_mem_malloc(64 * 64 * 2);
    png_to_texture("/rd/vipersm2.png", player.texture_pointer, PNG_FULL_ALPHA);
    
    // Set initial position to the center of the screen
    player.x = 320; // Half of 640 (screen width)
    player.y = 240; // Half of 480 (screen height)
    player.isalive = 1; // Set the ship as alive
}