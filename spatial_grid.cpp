#include "spatial_grid.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <climits>

SpatialGrid::SpatialGrid(float worldWidth, float worldHeight, int gridWidth, int gridHeight)
    : worldWidth(worldWidth), worldHeight(worldHeight), 
      gridWidth(gridWidth), gridHeight(gridHeight) {
    
    cellWidth = worldWidth / gridWidth;
    cellHeight = worldHeight / gridHeight;
    
    grid.resize(gridWidth * gridHeight);
    
    printf("SpatialGrid: Initialized %dx%d grid (%.1fx%.1f cells) for %.1fx%.1f world\n",
           gridWidth, gridHeight, cellWidth, cellHeight, worldWidth, worldHeight);
}

void SpatialGrid::clear() {
    for (auto& cell : grid) {
        cell.clear();
    }
}

void SpatialGrid::addObject(obj* object) {
    if (!object || !object->isalive) return;
    
    int gridX, gridY;
    getGridCoords(object->x, object->y, gridX, gridY);
    
    if (isValidGridCoord(gridX, gridY)) {
        int index = gridY * gridWidth + gridX;
        grid[index].push_back(object);
    }
}

std::vector<obj*> SpatialGrid::getObjectsInRange(float x, float y, float radius) const {
    std::vector<obj*> result;
    
    // Calculate grid range to check
    int minGridX = std::max(0, (int)((x - radius) / cellWidth));
    int maxGridX = std::min(gridWidth - 1, (int)((x + radius) / cellWidth));
    int minGridY = std::max(0, (int)((y - radius) / cellHeight));
    int maxGridY = std::min(gridHeight - 1, (int)((y + radius) / cellHeight));
    
    for (int gridY = minGridY; gridY <= maxGridY; gridY++) {
        for (int gridX = minGridX; gridX <= maxGridX; gridX++) {
            int index = gridY * gridWidth + gridX;
            
            for (obj* obj : grid[index]) {
                if (obj && obj->isalive) {
                    // Additional distance check for objects near cell boundaries
                    float dx = obj->x - x;
                    float dy = obj->y - y;
                    float distance = sqrt(dx * dx + dy * dy);
                    
                    if (distance <= radius) {
                        result.push_back(obj);
                    }
                }
            }
        }
    }
    
    return result;
}

std::vector<obj*> SpatialGrid::getPotentialCollisions(const obj* object) const {
    if (!object || !object->isalive) {
        return std::vector<obj*>();
    }
    
    // Use object's maximum dimension as search radius
    float maxDimension = std::max(object->imgX, object->imgY);
    float searchRadius = maxDimension; // Conservative estimate
    
    return getObjectsInRange(object->x, object->y, searchRadius);
}

int SpatialGrid::getTotalObjects() const {
    int total = 0;
    for (const auto& cell : grid) {
        total += cell.size();
    }
    return total;
}

void SpatialGrid::printGridStats() const {
    int minObjects = INT_MAX;
    int maxObjects = 0;
    int totalObjects = 0;
    int nonEmptyCells = 0;
    
    for (const auto& cell : grid) {
        int cellSize = cell.size();
        totalObjects += cellSize;
        
        if (cellSize > 0) {
            nonEmptyCells++;
            minObjects = std::min(minObjects, cellSize);
            maxObjects = std::max(maxObjects, cellSize);
        }
    }
    
    if (minObjects == INT_MAX) minObjects = 0;
    
    printf("SpatialGrid Stats: %d objects in %d/%d cells (min:%d, max:%d, avg:%.1f)\n",
           totalObjects, nonEmptyCells, gridWidth * gridHeight, 
           minObjects, maxObjects, 
           nonEmptyCells > 0 ? (float)totalObjects / nonEmptyCells : 0.0f);
}

int SpatialGrid::getGridIndex(float x, float y) const {
    int gridX, gridY;
    getGridCoords(x, y, gridX, gridY);
    
    if (!isValidGridCoord(gridX, gridY)) {
        return -1;
    }
    
    return gridY * gridWidth + gridX;
}

void SpatialGrid::getGridCoords(float x, float y, int& gridX, int& gridY) const {
    gridX = (int)(x / cellWidth);
    gridY = (int)(y / cellHeight);
}

bool SpatialGrid::isValidGridCoord(int gridX, int gridY) const {
    return gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight;
}

std::vector<int> SpatialGrid::getNeighborCells(int centerX, int centerY, int radius) const {
    std::vector<int> neighbors;
    
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int x = centerX + dx;
            int y = centerY + dy;
            
            if (isValidGridCoord(x, y)) {
                neighbors.push_back(y * gridWidth + x);
            }
        }
    }
    
    return neighbors;
}