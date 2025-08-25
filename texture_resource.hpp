#ifndef TEXTURE_RESOURCE_HPP
#define TEXTURE_RESOURCE_HPP

#include <kos.h>

class TextureResource {
public:
    TextureResource();
    TextureResource(int width, int height);
    ~TextureResource();
    
    // Move constructor and assignment (C++11)
    TextureResource(TextureResource&& other) noexcept;
    TextureResource& operator=(TextureResource&& other) noexcept;
    
    // Delete copy constructor and assignment to prevent double-free
    TextureResource(const TextureResource&) = delete;
    TextureResource& operator=(const TextureResource&) = delete;
    
    // Access methods
    pvr_ptr_t get() const { return texture_ptr; }
    pvr_ptr_t* getPtr() { return &texture_ptr; }
    bool isValid() const { return texture_ptr != nullptr; }
    
    // Manual management
    bool allocate(int width, int height);
    void release();
    
    // Implicit conversion for compatibility with existing code
    operator pvr_ptr_t() const { return texture_ptr; }
    
private:
    pvr_ptr_t texture_ptr;
    
    void cleanup();
};

#endif // TEXTURE_RESOURCE_HPP