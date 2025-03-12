#pragma once

#include "raylib.h"
#include <vector>
#include <queue>
#include "game/assets_manager.h"

// Tile types
enum TileType {
    TILE_FLOOR,
    TILE_WALL,
    TILE_CACTUS,
    TILE_WATER,
    TILE_LAVA
};

// Enemy types for spawning - reference the one in enemy.h
enum LevelEnemyType {
    ENEMY_SPAWN_BASIC = 0,
    ENEMY_SPAWN_ORC,
    ENEMY_SPAWN_GHOST,
    ENEMY_SPAWN_MUMMY,
    ENEMY_SPAWN_DEVIL,
    ENEMY_SPAWN_BOSS_COWBOY,
    ENEMY_SPAWN_BOSS_FECTOR
};

// Structure for spawn queue
struct SpawnQueueItem {
    int enemyType;
    Vector2 position;
    float spawnTime;
    
    // Comparison operator for priority queue (smaller times have higher priority)
    bool operator>(const SpawnQueueItem& rhs) const {
        return spawnTime > rhs.spawnTime;
    }
};

class Level {
public:
    Level();
    ~Level();
    
    bool loadMap(int levelNum);
    void update(float deltaTime);
    void draw(Texture2D backgroundTexture, int worldNumber, float dancingCactusTimer);
    
    // Check if a position is passable (for collision detection)
    bool isPassable(float x, float y) const;
    
    // Add an enemy to the spawn queue
    void addToSpawnQueue(int enemyType, Vector2 position, float delay);
    
    // Get the next enemy to spawn (returns false if none ready)
    bool getNextSpawn(int& enemyType, Vector2& position);
    
    // Getters
    Vector2 getPlayerStart() const { return playerStart; }
    int getLevelNumber() const { return levelNumber; }
    
    // Added missing methods
    Vector2 getPlayerStartPosition() const { return playerStart; }
    bool isSpawnQueueEmpty() const { return spawnQueue.empty(); }
    int getOffsetX() const { return offsetX; }
    int getOffsetY() const { return offsetY; }
    int getWidth() const { return mapWidth; }
    int getHeight() const { return mapHeight; }
    const std::vector<Vector2>& getEnemySpawnPoints() const { return enemySpawnPoints; }
    
private:
    // Level generation methods
    void generateLevel(int levelNum);
    void generateStandardLevel(int levelNum);
    void generateBossLevel(int bossType);
    void generateShopLevel(void);
    void addObstacles(int count);
    void setupSpawnPoints();
    
    // Level data
    int levelNumber;
    int mapWidth;
    int mapHeight;
    std::vector<std::vector<TileType>> mapData;
    Vector2 playerStart;
    
    // Drawing offset values for centering the map
    int offsetX;
    int offsetY;
    
    // Animation timers
    float waterAnimationTimer;
    float lavaAnimationTimer;
    
    // Spawn points
    std::vector<Vector2> enemySpawnPoints;
    std::vector<Vector2> powerupSpawnPoints;
    
    // Enemy spawn queue (priority queue sorted by spawn time)
    std::priority_queue<SpawnQueueItem, std::vector<SpawnQueueItem>, std::greater<SpawnQueueItem>> spawnQueue;
};