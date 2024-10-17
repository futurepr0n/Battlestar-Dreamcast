#include "menu_system.hpp"
#include <png/png.h>

extern char wfont[];

MenuSystem::MenuSystem() : selectedIndex(0), upPressed(false), downPressed(false) {
    loadFont();
}

void MenuSystem::addMenuItem(const std::string& item) {
    menuItems.push_back(item);
}

void MenuSystem::loadFont() {
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
    free(temp_tex);
}

void MenuSystem::drawMenuItem(const std::string& item, float x, float y, bool selected) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 256, 256, font_tex, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    float r = selected ? 1.0f : 0.7f;
    float g = selected ? 1.0f : 0.7f;
    float b = selected ? 1.0f : 0.7f;

    for (char c : item) {
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

void MenuSystem::draw() {
    pvr_wait_ready();
    pvr_scene_begin();

    pvr_list_begin(PVR_LIST_OP_POLY);
    // Draw background here if needed
    pvr_list_finish();

    pvr_list_begin(PVR_LIST_TR_POLY);
    float y = 200.0f;
    for (size_t i = 0; i < menuItems.size(); ++i) {
        drawMenuItem(menuItems[i], 320.0f - menuItems[i].length() * 4.0f, y, i == selectedIndex);
        y += 30.0f;
    }
    pvr_list_finish();

    pvr_scene_finish();
}

bool MenuSystem::handleInput(cont_state_t* state) {
    bool inputHandled = false;

    if (state->buttons & CONT_DPAD_UP || state->joyy < -10) {
        if (!upPressed) {
            if (selectedIndex > 0) {
                selectedIndex--;
                inputHandled = true;
            }
            upPressed = true;
        }
    } else {
        upPressed = false;
    }

    if (state->buttons & CONT_DPAD_DOWN || state->joyy > 10) {
        if (!downPressed) {
            if (selectedIndex < menuItems.size() - 1) {
                selectedIndex++;
                inputHandled = true;
            }
            downPressed = true;
        }
    } else {
        downPressed = false;
    }

    return inputHandled;
}

int MenuSystem::getSelectedIndex() const {
    return selectedIndex;
}

void showMainMenu() {
    MenuSystem menu;
    menu.addMenuItem("Start Game");
    menu.addMenuItem("Options");
    menu.addMenuItem("Exit");

    maple_device_t *cont;
    cont_state_t *state;
    uint8 prev_buttons = 0;

    while (1) {
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

        if (cont) {
            state = (cont_state_t *)maple_dev_status(cont);

            if (state) {
                if (menu.handleInput(state)) {
                    // Redraw the menu if input changed the selection
                    menu.draw();
                }

                // Check for button press (not hold)
                if ((state->buttons & CONT_A) && !(prev_buttons & CONT_A)) {
                    // Handle menu selection
                    switch (menu.getSelectedIndex()) {
                        case 0: // Start Game
                            return;
                        case 1: // Options
                            // Implement options menu
                            break;
                        case 2: // Exit
                            exit(0);
                    }
                }

                prev_buttons = state->buttons;
            }
        }

        menu.draw();
        timer_spin_sleep(20);
    }
}