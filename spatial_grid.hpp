#ifndef SPATIAL_GRID_HPP
#define SPATIAL_GRID_HPP

#include "loadobj.hpp"
#include "game_constants.hpp"
#include <vector>

class SpatialGrid {
public:
    SpatialGrid(float worldWidth = SCREEN_WIDTH, float worldHeight = SCREEN_HEIGHT, 
                int gridWidth = 8, int gridHeight = 6);
    
    void clear();
    void addObject(obj* object);
    std::vector<obj*> getObjectsInRange(float x, float y, float radius) const;
    std::vector<obj*> getPotentialCollisions(const obj* object) const;
    
    // Debug information
    int getTotalObjects() const;
    void printGridStats() const;
    
private:
    float worldWidth, worldHeight;
    int gridWidth, gridHeight;
    float cellWidth, cellHeight;
    
    std::vector<std::vector<obj*>> grid;
    
    int getGridIndex(float x, float y) const;
    void getGridCoords(float x, float y, int& gridX, int& gridY) const;
    bool isValidGridCoord(int gridX, int gridY) const;
    std::vector<int> getNeighborCells(int centerX, int centerY, int radius = 1) const;
};

#endif // SPATIAL_GRID_HPP