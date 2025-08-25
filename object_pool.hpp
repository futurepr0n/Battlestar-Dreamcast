#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include <vector>
#include <queue>
#include <functional>

template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t initialSize = 10);
    ~ObjectPool();
    
    T* acquire();
    void release(T* obj);
    void releaseAll();
    
    size_t getActiveCount() const;
    size_t getPoolSize() const;
    size_t getAvailableCount() const;
    
    // Optional: Set custom initialization function
    void setInitializer(std::function<void(T*)> init);
    void setCleanup(std::function<void(T*)> cleanup);
    
private:
    std::vector<T> objects;
    std::queue<T*> available;
    std::function<void(T*)> initializer;
    std::function<void(T*)> cleaner;
    
    void expandPool(size_t additionalSize);
};

template<typename T>
ObjectPool<T>::ObjectPool(size_t initialSize) {
    objects.reserve(initialSize);
    expandPool(initialSize);
}

template<typename T>
ObjectPool<T>::~ObjectPool() {
    // Cleanup is automatic with RAII
}

template<typename T>
T* ObjectPool<T>::acquire() {
    if (available.empty()) {
        expandPool(objects.size()); // Double the pool size
    }
    
    T* obj = available.front();
    available.pop();
    
    if (initializer) {
        initializer(obj);
    }
    
    return obj;
}

template<typename T>
void ObjectPool<T>::release(T* obj) {
    if (!obj) return;
    
    if (cleaner) {
        cleaner(obj);
    }
    
    available.push(obj);
}

template<typename T>
void ObjectPool<T>::releaseAll() {
    // Clear the available queue and repopulate with all objects
    while (!available.empty()) {
        available.pop();
    }
    
    for (auto& obj : objects) {
        if (cleaner) {
            cleaner(&obj);
        }
        available.push(&obj);
    }
}

template<typename T>
size_t ObjectPool<T>::getActiveCount() const {
    return objects.size() - available.size();
}

template<typename T>
size_t ObjectPool<T>::getPoolSize() const {
    return objects.size();
}

template<typename T>
size_t ObjectPool<T>::getAvailableCount() const {
    return available.size();
}

template<typename T>
void ObjectPool<T>::setInitializer(std::function<void(T*)> init) {
    initializer = init;
}

template<typename T>
void ObjectPool<T>::setCleanup(std::function<void(T*)> cleanup) {
    cleaner = cleanup;
}

template<typename T>
void ObjectPool<T>::expandPool(size_t additionalSize) {
    size_t oldSize = objects.size();
    objects.resize(oldSize + additionalSize);
    
    // Add new objects to available queue
    for (size_t i = oldSize; i < objects.size(); ++i) {
        available.push(&objects[i]);
    }
}

#endif // OBJECT_POOL_HPP