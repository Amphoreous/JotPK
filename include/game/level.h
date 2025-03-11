#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"
#include <vector>
#include <queue>
#include <functional> // Add this include for std::function

// Tile types
enum TileType {
    TILE_FLOOR,
    TILE_WALL,
    TILE_CACTUS,
    TILE_WATER,
    TILE_LAVA,
    TILE_COUNT
};

// Structure to define a spawn queue item
struct SpawnQueueItem {
    int enemyType;
    Vector2 position;
    float spawnTime;
};

// Comparison function for priority queue
struct SpawnQueueCompare {
    bool operator()(const SpawnQueueItem& a, const SpawnQueueItem& b) const {
        return a.spawnTime > b.spawnTime;  // Lower time = higher priority
    }
};

class Level {
public:
    Level();
    ~Level();
    
    // Core functionality
    bool loadMap(int levelNumber);
    void update(float deltaTime);
    void draw(Texture2D backgroundTexture, int worldNumber, float dancingCactusTimer);
    
    // Map properties
    int getWidth() const { return mapWidth; }
    int getHeight() const { return mapHeight; }
    bool isPassable(float x, float y) const;
    
    // Spawn management
    void addToSpawnQueue(int enemyType, Vector2 position, float delay);
    bool isSpawnQueueEmpty() const { return spawnQueue.empty(); }
    bool getNextSpawn(int& enemyType, Vector2& position);
    
    // Special map features
    Vector2 getPlayerStartPosition() const { return playerStart; }
    std::vector<Vector2> getEnemySpawnPoints() const { return enemySpawnPoints; }
    std::vector<Vector2> getPowerupSpawnPoints() const { return powerupSpawnPoints; }
    
    // Level-specific information
    bool isBossLevel() const { return levelNumber % 4 == 0; }
    bool isShopLevel() const { return levelNumber % 4 == 3; }
    
private:
    // Level data
    int levelNumber;
    int mapWidth;
    int mapHeight;
    std::vector<std::vector<TileType>> mapData;
    
    // Special positions
    Vector2 playerStart;
    std::vector<Vector2> enemySpawnPoints;
    std::vector<Vector2> powerupSpawnPoints;
    
    // Enemy spawn queue - use the SpawnQueueCompare struct instead of std::function
    std::priority_queue<SpawnQueueItem, std::vector<SpawnQueueItem>, SpawnQueueCompare> spawnQueue;
    
    // Animation timers for special tiles
    float waterAnimationTimer;
    float lavaAnimationTimer;
    
    // Helper methods
    void generateLevel(int levelNum);
    void generateBossLevel(int bossType);
    void generateShopLevel();
    void generateStandardLevel(int levelNum);
    void addObstacles(int count);
};

#endif // LEVEL_H