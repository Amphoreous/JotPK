#pragma once

#include "raylib.h"
#include <vector>
#include <queue>
#include "game/assets_manager.h"
#include "game/game_defs.h"

// Constants matching AbigailGame.cs
#define TILE_SIZE 48           // 48px tiles as in original
#define MAP_WIDTH 16          // 16x16 grid 
#define MAP_HEIGHT 16
#define WAVE_DURATION 80000    // Match original wave timing

// Tile types from AbigailGame.cs
enum AbigailTileTypes {
    MAP_BARRIER1 = 0,
    MAP_BARRIER2 = 1,
    MAP_ROCKY1 = 2,
    MAP_DESERT = 3,
    MAP_GRASSY = 4,
    MAP_CACTUS = 5,
    MAP_FENCE = 7,
    MAP_TRENCH1 = 8,
    MAP_TRENCH2 = 9,
    MAP_BRIDGE = 10
};

struct SpawnQueueItem {
    int enemyType;
    Vector2 position;
    float spawnTime;
    bool operator>(const SpawnQueueItem& rhs) const {
        return spawnTime > rhs.spawnTime;
    }
};

class Level {
public:
    Level();
    ~Level();
    
    bool loadMap(int waveNumber);
    void update(float deltaTime);
    void draw(Texture2D backgroundTexture, int worldTheme);
    bool isPassable(float worldX, float worldY) const;
    void addToSpawnQueue(int enemyType, Vector2 position, float delay);
    bool getNextSpawn(int& enemyType, Vector2& position);

    Vector2 getPlayerStartPosition() const { return playerStart; }
    bool isSpawnQueueEmpty() const { return spawnQueue.empty(); }
    int getWidth() const { return mapWidth; }
    int getHeight() const { return mapHeight; }
    const std::vector<Vector2>& getEnemySpawnPoints() const { return enemySpawnPoints; }
    void setOffset(float x, float y) { offsetX = x; offsetY = y; }

    Rectangle GetTileSourceRect(TileType tile, int worldNumber);

private:
    void initializeMap();
    void generateWaveMap(int wave);
    void createVictoryMap();
    void createBossArena();
    void generateShopLayout();
    void addWaveSpecificFeatures(int wave);
    void setupWaveSpawns(int wave);
    void clearSpawnQueue();
    Rectangle getTileSource(int x, int y, int worldTheme) const;

    int levelNumber;
    int mapWidth;
    int mapHeight;
    std::vector<std::vector<int>> mapData;
    Vector2 playerStart;
    float offsetX;
    float offsetY;
    
    // Animation timers
    float waterAnimationTimer;
    float lavaAnimationTimer;
    float dancingCactusTimer;
    float waveTimer;

    std::vector<Vector2> enemySpawnPoints;
    std::vector<Vector2> powerupSpawnPoints;
    std::priority_queue<SpawnQueueItem, std::vector<SpawnQueueItem>, std::greater<SpawnQueueItem>> spawnQueue;
};