#include <kos.h>
#include <mp3/sndserver.h>
#include "endgamesplash.hpp"
#include <png/png.h>
#include <zlib/zlib.h>
#include "game_state.hpp"
#include <string>

/* font texture */
pvr_ptr_t victory_font_tex;
pvr_ptr_t victory_back_tex;

extern char wfont[]; // Using same font as main game
char *victory_data;

extern "C" {
    int zlib_getlength(char*);
} 


/* init victory background */
void victory_back_init(void) {
    victory_back_tex = pvr_mem_malloc(512 * 512 * 2);
    png_to_texture("/rd/victory_background.png", victory_back_tex, PNG_NO_ALPHA);
}

/* init font specifically for victory screen */
static void victory_font_init(void) {
    int i, x, y, c;
    unsigned short * temp_tex;

    victory_font_tex = pvr_mem_malloc(256 * 256 * 2);
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

    pvr_txr_load_ex(temp_tex, victory_font_tex, 256, 256, PVR_TXRLOAD_16BPP);
    free(temp_tex);
}

static void victory_text_init(void) {
    const char* filename = "/rd/win.gz";
    int length = zlib_getlength(const_cast<char*>(filename));
    gzFile f;

    victory_data = (char *)malloc(length + 1);

    f = gzopen("/rd/win.gz", "r");
    gzread(f, victory_data, length);
    victory_data[length] = 0;
    gzclose(f);

    printf("Victory text length [%d]\n", length);
}

/* draw victory background */
static void draw_victory_back(void) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565, 512, 512, victory_back_tex, PVR_FILTER_BILINEAR);
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

/* draw character */
static void draw_victory_char(float x1, float y1, float z1, float a, float r, float g, float b, int c, float xs, float ys) {
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

/* draw string */
static void draw_victory_string(float x, float y, float z, float a, float r, float g, float b, char *str, float xs, float ys) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    float orig_x = x;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 256, 256, victory_font_tex, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    while(*str) {
        if(*str == '\n') {
            x = orig_x;
            y += 40;
            str++;
            continue;
        }

        draw_victory_char(x, y, z, a, r, g, b, *str++, xs, ys);
        x += 8 * xs;
    }
}

/* draw one victory frame */
static void draw_victory_frame(int y_pos, const std::string& player_name, int score) {
    pvr_wait_ready();
    pvr_scene_begin();

    pvr_list_begin(PVR_LIST_OP_POLY);
    draw_victory_back();
    pvr_list_finish();

    pvr_list_begin(PVR_LIST_TR_POLY);

    // Draw scrolling victory text
    draw_victory_string(0, y_pos % 1720 + 440, 3, 1, 1, 1, 1, victory_data, 2, 2);

    // Draw player stats (fixed position)
    std::string stats = "Congratulations " + player_name + "\nFinal Score: " + std::to_string(score);
    draw_victory_string(200, 200, 3, 1, 1, 1, 0, const_cast<char*>(stats.c_str()), 2, 2);

    pvr_list_finish();
    pvr_scene_finish();
}

void showVictoryScreen() {
    int done = 0;
    int y = 0;
    
    // Get player info
    std::string player_name = GameState::getInstance().getPlayerName();
    int final_score = GameState::getInstance().getScore();

    /* init font */
    victory_font_init();
    
    /* init background */
    victory_back_init();

    /* init victory text */
    victory_text_init();

    /* init soundstream and mp3 stuff */
    snd_stream_init();
    mp3_init();

    mp3_start("/rd/victory.mp3", 0);

    /* keep drawing frames until start is pressed */
    while(!done) {
        MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)
        if(st->buttons & CONT_START)
            done = 1;
        MAPLE_FOREACH_END()

        draw_victory_frame(y, player_name, final_score);
        y--;
    }

    /* Clean up */
    mp3_stop();
    free(victory_data);

    return;
}