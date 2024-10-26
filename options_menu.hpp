#ifndef OPTIONS_MENU_HPP
#define OPTIONS_MENU_HPP

#include <kos.h>
#include <string>

class OptionsMenu {
public:
    OptionsMenu();
    bool run();

private:
    enum class SelectedOption {
        EnemyWaves,
        Difficulty,
        Back
    };

    static const int INPUT_DELAY = 15;  // Frames to wait between inputs
    static const int EDITING_INPUT_DELAY = 8;  // Slightly faster when editing values

    SelectedOption selectedOption;
    bool isEditing;
    pvr_ptr_t font_tex;
    
    // Input handling variables
    int inputDelayCounter;
    bool upPressed;
    bool downPressed;
    uint32_t prevButtons;

    void draw();
    bool handleInput();
    void loadFont();
    void drawMenuOption(const std::string& text, float x, float y, bool isSelected, bool isEditing = false);
    std::string getDifficultyString() const;
    void resetInputState();
    void saveSettings();  // Add method to save settings
};

#endif // OPTIONS_MENU_HPP
