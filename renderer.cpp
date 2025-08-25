#include "renderer.hpp"
#include "blitobj.hpp"
#include "movement.hpp"
#include "game_utils.hpp"
#include "game_state.hpp"
#include "font_manager.hpp"
#include <string>
#include <png/png.h>

pvr_ptr_t font_tex;  // Add this line
extern uint8 wfont[];

extern obj background;
extern obj starfield;
extern obj player;
extern obj battlestar;

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

void draw_ship(const obj& player) {
    blitObj(player);
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

void load_font_texture() {
    auto& fontManager = FontManager::getInstance();
    if (!fontManager.isInitialized()) {
        fontManager.initialize();
    }
    font_tex = fontManager.getFontTexture();
}

void draw_text(const char* text, float x, float y, float r = 1.0f, float g = 1.0f, float b = 1.0f) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 256, 256, font_tex, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    while (*text) {
        char c = *text++;
        int ix = (c % 32) * 8;
        int iy = (c / 32) * 16;
        float u1 = (ix + 0.5f) * 1.0f / 256.0f;
        float v1 = (iy + 0.5f) * 1.0f / 256.0f;
        float u2 = (ix + 7.5f) * 1.0f / 256.0f;
        float v2 = (iy + 15.5f) * 1.0f / 256.0f;

        vert.flags = PVR_CMD_VERTEX;
        vert.x = x;
        vert.y = y + 16.0f;
        vert.z = 1.0f;
        vert.u = u1;
        vert.v = v2;
        vert.argb = PVR_PACK_COLOR(1.0f, r, g, b);
        vert.oargb = 0;
        pvr_prim(&vert, sizeof(vert));

        vert.y = y;
        vert.v = v1;
        pvr_prim(&vert, sizeof(vert));

        vert.x = x + 8.0f;
        vert.y = y + 16.0f;
        vert.u = u2;
        vert.v = v2;
        pvr_prim(&vert, sizeof(vert));

        vert.flags = PVR_CMD_VERTEX_EOL;
        vert.y = y;
        vert.v = v1;
        pvr_prim(&vert, sizeof(vert));

        x += 8.0f;
    }
}

void draw_player_info() {
    std::string playerName = GameState::getInstance().getPlayerName();
    int score = GameState::getInstance().getScore();
    
    std::string displayText = playerName + " - Score: " + std::to_string(score);
    
    draw_text(displayText.c_str(), 10, 10);
}

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
    draw_ship(player);
    //blitEnemies(deltaTime);
    //blitObj(battlestar);
    draw_player_info();  // Add this line to display player info
    pvr_list_finish();
    pvr_scene_finish();

    x_offset -= 1.0f;
    if (x_offset <= -640.0f) {
        x_offset += 640.0f;
    }
}