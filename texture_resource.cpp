#include "texture_resource.hpp"
#include <iostream>

TextureResource::TextureResource() : texture_ptr(nullptr) {
}

TextureResource::TextureResource(int width, int height) : texture_ptr(nullptr) {
    allocate(width, height);
}

TextureResource::~TextureResource() {
    cleanup();
}

TextureResource::TextureResource(TextureResource&& other) noexcept : texture_ptr(other.texture_ptr) {
    other.texture_ptr = nullptr;
}

TextureResource& TextureResource::operator=(TextureResource&& other) noexcept {
    if (this != &other) {
        cleanup();
        texture_ptr = other.texture_ptr;
        other.texture_ptr = nullptr;
    }
    return *this;
}

bool TextureResource::allocate(int width, int height) {
    if (texture_ptr) {
        cleanup(); // Clean up existing texture first
    }
    
    size_t size = width * height * 2; // 16-bit textures
    texture_ptr = pvr_mem_malloc(size);
    
    if (!texture_ptr) {
        printf("TextureResource: Failed to allocate %dx%d texture (%zu bytes)\n", 
               width, height, size);
        return false;
    }
    
    return true;
}

void TextureResource::release() {
    cleanup();
}

void TextureResource::cleanup() {
    if (texture_ptr) {
        pvr_mem_free(texture_ptr);
        texture_ptr = nullptr;
    }
}