#ifndef NAME_SELECTION_HPP
#define NAME_SELECTION_HPP

#include <kos.h>
#include <string>
#include <vector>

class NameSelection {
public:
    NameSelection();
    std::string run();

private:
    static const int MAX_NAME_LENGTH = 15;
    static const std::vector<std::vector<std::string>> CHARACTERS;

    std::string playerName;
    int cursorPosition;
    int selectedRow;
    int selectedCol;
    int inputDelay;

    void draw();
    bool handleInput(cont_state_t* state, uint8 prev_buttons);
    void drawCharacter(char c, float x, float y, bool selected);
    void drawString(const std::string& str, float x, float y, bool selected);
    void loadFont();
    pvr_ptr_t font_tex;
};

#endif // NAME_SELECTION_HPP