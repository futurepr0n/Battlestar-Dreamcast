#ifndef MENU_SYSTEM_HPP
#define MENU_SYSTEM_HPP

#include <kos.h>
#include <vector>
#include <string>
#include <iostream>

class MenuSystem {
public:
    MenuSystem();
    void addMenuItem(const std::string& item);
    void draw();
    bool handleInput(cont_state_t* state);
    int getSelectedIndex() const;

private:
    std::vector<std::string> menuItems;
    int selectedIndex;
    pvr_ptr_t font_tex;
    bool upPressed;
    bool downPressed;

    void loadFont();
    void drawMenuItem(const std::string& item, float x, float y, bool selected);
};

std::string showMainMenu();

#endif // MENU_SYSTEM_HPP