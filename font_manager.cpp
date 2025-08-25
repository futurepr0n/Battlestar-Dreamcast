#include "font_manager.hpp"
#include <cstring>
#include <iostream>

bool FontManager::initialize() {
    if (initialized) {
        return true;
    }
    
    if (!fontTexture.allocate(256, 256)) {
        printf("FontManager: Failed to allocate font texture\n");
        return false;
    }
    
    createFontTexture();
    initialized = true;
    
    printf("FontManager: Font system initialized\n");
    return true;
}

void FontManager::cleanup() {
    if (initialized) {
        fontTexture.release();
        initialized = false;
        printf("FontManager: Font system cleaned up\n");
    }
}

pvr_ptr_t FontManager::getFontTexture() const {
    return fontTexture.get();
}

bool FontManager::isInitialized() const {
    return initialized && fontTexture.isValid();
}

void FontManager::createFontTexture() {
    // Create temporary buffer for font processing
    unsigned short* temp_tex = (unsigned short*)malloc(256 * 128 * 2);
    if (!temp_tex) {
        printf("FontManager: Failed to allocate temporary texture buffer\n");
        return;
    }
    
    // Process font data from wfont binary
    loadFontFromBinary(reinterpret_cast<const char*>(wfont), 256 * 128);
    
    // Copy processed font data to PVR texture
    memcpy(temp_tex, wfont, 256 * 128 * 2);
    
    // Upload to PVR memory
    pvr_txr_load_ex(temp_tex, fontTexture.get(), 256, 256, PVR_TXRLOAD_16BPP);
    
    free(temp_tex);
}

void FontManager::loadFontFromBinary(const char* fontData, size_t dataSize) {
    // Font loading logic - this would process the binary font data
    // For now, we'll use the existing wfont data directly
    // This method could be expanded to support different font formats
    
    if (!fontData || dataSize == 0) {
        printf("FontManager: Invalid font data provided\n");
        return;
    }
    
    printf("FontManager: Loaded font data (%zu bytes)\n", dataSize);
}