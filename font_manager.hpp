#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP

#include <kos.h>
#include "texture_resource.hpp"

class FontManager {
public:
    static FontManager& getInstance() {
        static FontManager instance;
        return instance;
    }
    
    bool initialize();
    void cleanup();
    
    pvr_ptr_t getFontTexture() const;
    bool isInitialized() const;
    
    // Font rendering utility (can be used by all systems)
    void loadFontFromBinary(const char* fontData, size_t dataSize);
    
private:
    FontManager() = default;
    ~FontManager() = default;
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    
    TextureResource fontTexture;
    bool initialized = false;
    
    void createFontTexture();
};

extern uint8 wfont[];  // Font data from wfont.o

#endif // FONT_MANAGER_HPP