#include "options_menu.hpp"
#include "game_settings.hpp"
#include <string>

extern char wfont[];

OptionsMenu::OptionsMenu() 
    : selectedOption(SelectedOption::EnemyWaves)
    , isEditing(false)
    , inputDelayCounter(0)
    , upPressed(false)
    , downPressed(false)
    , prevButtons(0) {
    loadFont();
}

void OptionsMenu::resetInputState() {
    upPressed = false;
    downPressed = false;
    inputDelayCounter = 0;
}

bool OptionsMenu::run() {
    resetInputState();
    
    while (true) {
        if (handleInput()) {
            return true;  // Signal to return to previous menu
        }
        draw();
        timer_spin_sleep(20);
    }
    return false;
}


bool OptionsMenu::handleInput() {
    maple_device_t* cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if (!cont) return false;

    cont_state_t* state = (cont_state_t*)maple_dev_status(cont);
    if (!state) return false;

    uint32_t pressed = state->buttons & ~prevButtons;
    bool isUp = (state->buttons & CONT_DPAD_UP) || (state->joyy < -10);
    bool isDown = (state->buttons & CONT_DPAD_DOWN) || (state->joyy > 10);
    
    // Handle A and B button presses
    if (!isEditing) {
        if (pressed & CONT_A) {
            if (selectedOption == SelectedOption::Back) {
                saveSettings();  // Save settings before returning
                return true;  // Return to previous menu
            } else {
                isEditing = true;
                resetInputState();
            }
        } else if (pressed & CONT_B) {  // Allow B button to exit menu as well
            saveSettings();  // Save settings before returning
            return true;  // Return to previous menu
        }
    } else {
        if (pressed & CONT_B) {
            isEditing = false;
            resetInputState();
        }
    }

    // Handle directional input with delay
    int currentDelay = isEditing ? EDITING_INPUT_DELAY : INPUT_DELAY;

    if (isUp) {
        if (!upPressed && inputDelayCounter == 0) {
            if (!isEditing) {
                if (selectedOption == SelectedOption::Difficulty) {
                    selectedOption = SelectedOption::EnemyWaves;
                } else if (selectedOption == SelectedOption::Back) {
                    selectedOption = SelectedOption::Difficulty;
                }
            } else {
                if (selectedOption == SelectedOption::EnemyWaves) {
                    auto& settings = GameSettings::getInstance();
                    settings.setEnemyWaves(settings.getEnemyWaves() + 1);
                } else if (selectedOption == SelectedOption::Difficulty) {
                    GameSettings::getInstance().cycleDifficulty();
                }
            }
            upPressed = true;
            inputDelayCounter = currentDelay;
        }
    } else {
        upPressed = false;
    }

    if (isDown) {
        if (!downPressed && inputDelayCounter == 0) {
            if (!isEditing) {
                if (selectedOption == SelectedOption::EnemyWaves) {
                    selectedOption = SelectedOption::Difficulty;
                } else if (selectedOption == SelectedOption::Difficulty) {
                    selectedOption = SelectedOption::Back;
                }
            } else {
                if (selectedOption == SelectedOption::EnemyWaves) {
                    auto& settings = GameSettings::getInstance();
                    settings.setEnemyWaves(settings.getEnemyWaves() - 1);
                } else if (selectedOption == SelectedOption::Difficulty) {
                    GameSettings::getInstance().cycleDifficultyReverse();
                }
            }
            downPressed = true;
            inputDelayCounter = currentDelay;
        }
    } else {
        downPressed = false;
    }

    // Update delay counter
    if (inputDelayCounter > 0) {
        inputDelayCounter--;
    }

    prevButtons = state->buttons;
    return false;
}

void OptionsMenu::draw() {
    pvr_wait_ready();
    pvr_scene_begin();

    pvr_list_begin(PVR_LIST_TR_POLY);

    // Draw title
    drawMenuOption("OPTIONS", 280.0f, 160.0f, false);

    // Draw enemy waves option
    std::string wavesText = "Enemy Waves: " + std::to_string(GameSettings::getInstance().getEnemyWaves());
    drawMenuOption(wavesText, 200.0f, 220.0f, 
                  selectedOption == SelectedOption::EnemyWaves,
                  isEditing && selectedOption == SelectedOption::EnemyWaves);

    // Draw difficulty option
    std::string diffText = "Difficulty: " + getDifficultyString();
    drawMenuOption(diffText, 200.0f, 260.0f,
                  selectedOption == SelectedOption::Difficulty,
                  isEditing && selectedOption == SelectedOption::Difficulty);

    // Draw back option
    drawMenuOption("Back", 200.0f, 300.0f, selectedOption == SelectedOption::Back);

    // Draw controls hint
    if (isEditing) {
        drawMenuOption("Press B to finish editing", 200.0f, 340.0f, false);
    } else {
        drawMenuOption("Press A to edit option", 200.0f, 340.0f, false);
    }

    pvr_list_finish();
    pvr_scene_finish();
}

std::string OptionsMenu::getDifficultyString() const {
    switch (GameSettings::getInstance().getDifficulty()) {
        case Difficulty::Easy:
            return "Easy";
        case Difficulty::Medium:
            return "Medium";
        case Difficulty::Hard:
            return "Hard";
        default:
            return "Unknown";
    }
}

void OptionsMenu::drawMenuOption(const std::string& text, float x, float y, bool isSelected, bool isEditing) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 256, 256, font_tex, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    float r = isSelected ? (isEditing ? 1.0f : 0.8f) : 0.6f;
    float g = isSelected ? (isEditing ? 0.5f : 0.8f) : 0.6f;
    float b = isSelected ? (isEditing ? 0.5f : 0.8f) : 0.6f;

    for (char c : text) {
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

void OptionsMenu::loadFont() {
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

void OptionsMenu::saveSettings() {
    // Settings are automatically saved in GameSettings singleton
    // But we could add additional saving logic here if needed
    // For example, saving to a file or flash memory
}