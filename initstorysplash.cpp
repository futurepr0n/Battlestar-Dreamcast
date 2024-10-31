#include <kos.h>
#include <mp3/sndserver.h>
#include <modplug/stdafx.h>
#include <modplug/sndfile.h>
#include "initstorysplash.hpp"
#include <png/png.h>

#include <zlib/zlib.h>


long bitrateold, bitratenew;

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);

/* font data */
extern char wfont[];


extern "C" {
    int zlib_getlength(char*);
} 

/* textures */
pvr_ptr_t font_tex2;
pvr_ptr_t back_tex;
pvr_ptr_t bsg_back_tex;

char *data;

/** MikMod Stuff **/


 /*  MikMod Stuff    */
    uint8 *mod_buffer;
    uint32 hnd;



/** MikMod Stuff **/


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

    font_tex2 = pvr_mem_malloc(256 * 256 * 2);
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

    pvr_txr_load_ex(temp_tex, font_tex2, 256, 256, PVR_TXRLOAD_16BPP);
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

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 256, 256, font_tex2, PVR_FILTER_BILINEAR);
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

void initStorySplash(){
    int done = 0;
	 /* init background */
     /* init font */
    font_init();
    
    /* init background */
    back_init();

    /* init text */
    text_init();
     /* Clean up any existing sound stream first */
    snd_stream_shutdown();

    /* init soundstream and mp3 stuff (from mp3test) */
    snd_stream_init();

    //mp3_shutdown(); 
    mp3_init();

     /* Start the theme music */
    if (mp3_start("/rd/theme.mp3", 0) != 0) {
        printf("Failed to start theme music\n");
    } else {
        printf("Theme music started successfully\n");
    }

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

    // Free memory
    if (back_tex) {
        pvr_mem_free(back_tex);
        back_tex = nullptr;
    }
    if (font_tex2) {
        pvr_mem_free(font_tex2);
        font_tex2 = nullptr;
    }
    if (data) {
        free(data);
        data = nullptr;
    }


    return;

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