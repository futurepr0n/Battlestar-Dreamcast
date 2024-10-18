#include "name_selection.hpp"
#include <png/png.h>
#include <cmath>

extern char wfont[];

const std::vector<std::vector<std::string>> NameSelection::CHARACTERS = {
    {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M"},
    {"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"},
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"},
    {"<", ">", "Del", "End"}
};

NameSelection::NameSelection() : cursorPosition(0), selectedRow(0), selectedCol(0), inputDelay(30) {
    loadFont();
}

std::string NameSelection::run() {
    maple_device_t *cont;
    cont_state_t *state;
    uint8 prev_buttons = 0;

    while (true) {
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

        if (cont) {
            state = (cont_state_t *)maple_dev_status(cont);

            if (state) {
                if (inputDelay > 0) {
                    inputDelay--;
                } else if (handleInput(state, prev_buttons)) {
                    draw();
                }

                if ((state->buttons & CONT_A) && !(prev_buttons & CONT_A) && inputDelay == 0) {
                    std::string selectedChar = CHARACTERS[selectedRow][selectedCol];
                    if (selectedChar == "End") {
                        return playerName;
                    } else if (selectedChar == "Del") {
                        if (!playerName.empty() && cursorPosition > 0) {
                            playerName.erase(cursorPosition - 1, 1);
                            cursorPosition--;
                        }
                    } else if (selectedChar == "<") {
                        if (cursorPosition > 0) cursorPosition--;
                    } else if (selectedChar == ">") {
                        if (cursorPosition < playerName.length()) cursorPosition++;
                    } else if (playerName.length() < MAX_NAME_LENGTH) {
                        if (cursorPosition == playerName.length()) {
                            playerName += selectedChar;
                        } else {
                            playerName.insert(cursorPosition, selectedChar);
                        }
                        cursorPosition++;
                    }
                }

                prev_buttons = state->buttons;
            }
        }

        draw();
        timer_spin_sleep(20);
    }
}

void NameSelection::draw() {
    pvr_wait_ready();
    pvr_scene_begin();

    pvr_list_begin(PVR_LIST_OP_POLY);
    // Draw background here if needed
    pvr_list_finish();

    pvr_list_begin(PVR_LIST_TR_POLY);

    // Draw player name
    float y = 100.0f;
    for (size_t i = 0; i < MAX_NAME_LENGTH; i++) {
        char c = (i < playerName.length()) ? playerName[i] : '_';
        drawCharacter(c, 320.0f - (MAX_NAME_LENGTH * 4.0f) + (i * 16.0f), y, i == cursorPosition);
    }

    // Draw character selection
    y = 200.0f;
    for (size_t row = 0; row < CHARACTERS.size(); row++) {
        float x = 320.0f - (CHARACTERS[row].size() * 8.0f);
        for (size_t col = 0; col < CHARACTERS[row].size(); col++) {
            drawString(CHARACTERS[row][col], x, y, row == selectedRow && col == selectedCol);
            x += CHARACTERS[row][col].length() * 16.0f + 8.0f; // Add some spacing between options
        }
        y += 30.0f; // Move to the next line
    }

    pvr_list_finish();
    pvr_scene_finish();
}


bool NameSelection::handleInput(cont_state_t* state, uint8 prev_buttons) {
    bool inputHandled = false;

    if ((state->buttons & CONT_DPAD_LEFT || state->joyx < -10) && !(prev_buttons & CONT_DPAD_LEFT)) {
        if (selectedCol > 0) {
            selectedCol--;
            inputHandled = true;
        } else if (selectedRow > 0) {
            selectedRow--;
            selectedCol = CHARACTERS[selectedRow].size() - 1;
            inputHandled = true;
        }
    }
    if ((state->buttons & CONT_DPAD_RIGHT || state->joyx > 10) && !(prev_buttons & CONT_DPAD_RIGHT)) {
        if (selectedCol < CHARACTERS[selectedRow].size() - 1) {
            selectedCol++;
            inputHandled = true;
        } else if (selectedRow < CHARACTERS.size() - 1) {
            selectedRow++;
            selectedCol = 0;
            inputHandled = true;
        }
    }
    if ((state->buttons & CONT_DPAD_UP || state->joyy < -10) && !(prev_buttons & CONT_DPAD_UP)) {
        if (selectedRow > 0) {
            selectedRow--;
            selectedCol = std::min(selectedCol, static_cast<int>(CHARACTERS[selectedRow].size() - 1));
            inputHandled = true;
        }
    }
    if ((state->buttons & CONT_DPAD_DOWN || state->joyy > 10) && !(prev_buttons & CONT_DPAD_DOWN)) {
        if (selectedRow < CHARACTERS.size() - 1) {
            selectedRow++;
            selectedCol = std::min(selectedCol, static_cast<int>(CHARACTERS[selectedRow].size() - 1));
            inputHandled = true;
        }
    }

    return inputHandled;
}

void NameSelection::drawCharacter(char c, float x, float y, bool selected) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 256, 256, font_tex, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    float r = selected ? 1.0f : 0.7f;
    float g = selected ? 1.0f : 0.7f;
    float b = selected ? 1.0f : 0.7f;

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
}

void NameSelection::drawString(const std::string& str, float x, float y, bool selected) {
    for (char c : str) {
        drawCharacter(c, x, y, selected);
        x += 16.0f;
    }
}

void NameSelection::loadFont() {
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