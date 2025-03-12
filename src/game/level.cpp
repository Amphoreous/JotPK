#include "game/level.h"
#include "game/game_defs.h"
#include "game/enemy.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <random>

// Constants for level generation
#define TILE_SIZE 32
#define MAP_WIDTH 24  // 768 / TILE_SIZE
#define MAP_HEIGHT 24 // 768 / TILE_SIZE
#define ANIMATION_SPEED 0.2f
#define MAX_OBSTACLES 30

Level::Level() :
    levelNumber(0),
    mapWidth(MAP_WIDTH),
    mapHeight(MAP_HEIGHT),
    playerStart({384.0f, 384.0f}),
    waterAnimationTimer(0),
    lavaAnimationTimer(0)
{
    // Initialize empty map
    mapData.resize(mapHeight, std::vector<TileType>(mapWidth, TILE_FLOOR));
}

Level::~Level() {
    // Clear any allocated resources
    while (!spawnQueue.empty()) {
        spawnQueue.pop();
    }
}

bool Level::loadMap(int levelNum) {
    // Store level number
    levelNumber = levelNum;
    
    // Clear existing data
    mapData.clear();
    mapData.resize(mapHeight, std::vector<TileType>(mapWidth, TILE_FLOOR));
    enemySpawnPoints.clear();
    powerupSpawnPoints.clear();
    
    while (!spawnQueue.empty()) {
        spawnQueue.pop();
    }
    
    // Generate the appropriate level
    generateLevel(levelNum);
    
    return true;
}

void Level::generateLevel(int levelNum) {
    // Determine type of level based on level number
    if (levelNum % 4 == 0) {
        // Boss level
        int bossType = (levelNum / 4) % 2;  // Alternates between 0 (Cowboy) and 1 (Fector)
        generateBossLevel(bossType);
    }
    else if (levelNum % 4 == 3) {
        // Shop level before boss
        generateShopLevel();
    }
    else {
        // Standard level
        generateStandardLevel(levelNum);
    }
}

void Level::generateStandardLevel(int levelNum) {
    // Set player start position in center
    playerStart = {mapWidth * TILE_SIZE / 2.0f, mapHeight * TILE_SIZE / 2.0f};
    
    // Clear current map data
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            mapData[y][x] = TILE_FLOOR;
        }
    }
    
    // Create border walls - always present in all levels
    for (int y = 0; y < mapHeight; y++) {
        mapData[y][0] = TILE_WALL;
        mapData[y][mapWidth - 1] = TILE_WALL;
    }
    
    for (int x = 0; x < mapWidth; x++) {
        mapData[0][x] = TILE_WALL;
        mapData[mapHeight - 1][x] = TILE_WALL;
    }

    // For level 1 (whichWave = 0), create a very simple layout
    if (levelNum == 1) {
        // Leave the middle mostly open with just a few obstacles
        // Add some cacti/obstacles around the edges
        for (int i = 4; i < mapWidth - 4; i += 5) {
            mapData[3][i] = TILE_CACTUS;
            mapData[mapHeight - 4][i] = TILE_CACTUS;
        }
        
        // Add a few walls for obstacles
        mapData[8][6] = TILE_WALL;
        mapData[8][10] = TILE_WALL;
        mapData[8][14] = TILE_WALL;
        mapData[8][18] = TILE_WALL;
        
        mapData[16][6] = TILE_WALL;
        mapData[16][10] = TILE_WALL;
        mapData[16][14] = TILE_WALL;
        mapData[16][18] = TILE_WALL;
    } else {
        // For later levels, generate more complex patterns based on original game
        // This pattern repeats what the original game does for wave 2-3
        for (int i = 3; i < mapWidth - 3; i += 4) {
            for (int j = 5; j < mapHeight - 5; j += 8) {
                if ((i + j) % 2 == 0) {
                    mapData[j][i] = TILE_CACTUS;
                }
                else {
                    mapData[j][i] = TILE_WALL;
                }
            }
        }
    }
    
    // Make sure the center area around player start is clear
    int clearRadius = 3;
    int centerX = static_cast<int>(playerStart.x / TILE_SIZE);
    int centerY = static_cast<int>(playerStart.y / TILE_SIZE);
    
    for (int y = centerY - clearRadius; y <= centerY + clearRadius; y++) {
        for (int x = centerX - clearRadius; x <= centerX + clearRadius; x++) {
            if (y >= 0 && y < mapHeight && x >= 0 && x < mapWidth) {
                if (mapData[y][x] != TILE_FLOOR) {
                    mapData[y][x] = TILE_FLOOR;
                }
            }
        }
    }

    // Setup spawn points similar to the original game
    setupSpawnPoints();
}

void Level::setupSpawnPoints() {
    // Clear existing spawn points
    enemySpawnPoints.clear();
    powerupSpawnPoints.clear();
    
    // Add enemy spawn points at each corner with an offset from the wall
    enemySpawnPoints.push_back(Vector2{TILE_SIZE * 2.0f, TILE_SIZE * 2.0f}); // Top-left
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE - TILE_SIZE * 3.0f, TILE_SIZE * 2.0f}); // Top-right
    enemySpawnPoints.push_back(Vector2{TILE_SIZE * 2.0f, mapHeight * TILE_SIZE - TILE_SIZE * 3.0f}); // Bottom-left
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE - TILE_SIZE * 3.0f, mapHeight * TILE_SIZE - TILE_SIZE * 3.0f}); // Bottom-right
    
    // Add middle spawn points on each edge
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 2.0f, TILE_SIZE * 2.0f}); // Top-middle
    enemySpawnPoints.push_back(Vector2{TILE_SIZE * 2.0f, mapHeight * TILE_SIZE / 2.0f}); // Left-middle
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE - TILE_SIZE * 3.0f, mapHeight * TILE_SIZE / 2.0f}); // Right-middle
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 2.0f, mapHeight * TILE_SIZE - TILE_SIZE * 3.0f}); // Bottom-middle
    
    // Set powerup spawn locations (similar to original game positions)
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 3.0f, mapHeight * TILE_SIZE / 3.0f});
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE * 2.0f / 3.0f, mapHeight * TILE_SIZE / 3.0f});
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 3.0f, mapHeight * TILE_SIZE * 2.0f / 3.0f});
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE * 2.0f / 3.0f, mapHeight * TILE_SIZE * 2.0f / 3.0f});
}

void Level::generateBossLevel(int bossType) {
    // Set player start position
    playerStart.x = static_cast<float>(mapWidth * TILE_SIZE) / 2.0f;
    playerStart.y = static_cast<float>(mapHeight * TILE_SIZE) * 3.0f / 4.0f;
    
    // Boss levels have simpler layouts with fewer obstacles
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (x == 0 || y == 0 || x == mapWidth - 1 || y == mapHeight - 1) {
                mapData[y][x] = TILE_WALL;
            }
        }
    }
    
    // Add a few obstacles
    addObstacles(10);
    
    // Set boss spawn position at top middle
    Vector2 bossSpawn;
    bossSpawn.x = static_cast<float>(mapWidth * TILE_SIZE) / 2.0f;
    bossSpawn.y = static_cast<float>(TILE_SIZE) * 3.0f;
    
    // Add boss to spawn queue
    int bossEnemyType = (bossType == 0) ? ENEMY_SPAWN_BOSS_COWBOY : ENEMY_SPAWN_BOSS_FECTOR;
    addToSpawnQueue(bossEnemyType, bossSpawn, 2.0f);  // Spawn boss after 2 seconds
    
    // Add some minions
    int minions = 6 + bossType * 2;  // 6 for cowboy, 8 for fector
    
    for (int i = 0; i < minions; i++) {
        // Random position around the boss
        float angle = (float)i / minions * 2.0f * PI;
        Vector2 minionPos = {
            bossSpawn.x + cosf(angle) * TILE_SIZE * 3.0f,
            bossSpawn.y + sinf(angle) * TILE_SIZE * 3.0f
        };
        
        // Choose appropriate minion type
        int minionType = (bossType == 0) ? ENEMY_ORC : ENEMY_GHOST;
        
        // Add to spawn queue with increasing delays
        addToSpawnQueue(minionType, minionPos, 4.0f + i * 0.5f);
    }
}

void Level::generateShopLevel(void) {
    // Set player start position
    playerStart = {mapWidth * TILE_SIZE / 2.0f, mapHeight * TILE_SIZE * 3.0f / 4.0f};
    
    // Shop levels have walls that create a small enclosed area
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            // Create outer walls
            if (x == 0 || y == 0 || x == mapWidth - 1 || y == mapHeight - 1) {
                mapData[y][x] = TILE_WALL;
            }
            
            // Create inner shop area
            if (x >= mapWidth/3 && x <= mapWidth*2/3 && 
                y >= mapHeight/3 && y <= mapHeight*2/3) {
                
                // Make interior floor
                mapData[y][x] = TILE_FLOOR;
                
                // Make walls around the shop
                if (x == mapWidth/3 || x == mapWidth*2/3 || 
                    y == mapHeight/3 || y == mapHeight*2/3) {
                    
                    mapData[y][x] = TILE_WALL;
                    
                    // Create entrance at bottom
                    if (y == mapHeight*2/3 && x >= mapWidth/3 + 2 && x <= mapWidth*2/3 - 2) {
                        mapData[y][x] = TILE_FLOOR;
                    }
                }
            }
        }
    }
    
    // No enemies in shop level
    enemySpawnPoints.clear();
    
    // Add merchant position (centered in shop)
    powerupSpawnPoints.push_back({mapWidth * TILE_SIZE / 2.0f, mapHeight * TILE_SIZE * 2.0f / 5.0f});
    
    // Shop levels don't spawn enemies
}

void Level::addObstacles(int count) {
    // Add random obstacles in valid locations
    for (int i = 0; i < count; i++) {
        int attempts = 0;
        int x, y;
        float dist;
        
        // Try to find a valid position
        while (attempts < 100) {
            // Generate random position (avoid borders)
            x = GetRandomValue(2, mapWidth - 3);
            y = GetRandomValue(2, mapHeight - 3);
            
            // Calculate distance to player start
            dist = Vector2Distance(Vector2{(float)x * TILE_SIZE, (float)y * TILE_SIZE}, playerStart);
            
            // If this is a floor tile and far enough from player start
            if (mapData[y][x] == TILE_FLOOR && dist > TILE_SIZE * 3) {
                break;
            }
            
            attempts++;
        }
        
        // If a valid position was found, place obstacle
        if (attempts < 100) {
            // Choose obstacle type
            int obstacleType = GetRandomValue(0, 10);
            if (obstacleType < 7) {
                mapData[y][x] = TILE_WALL;
            } else if (obstacleType < 9) {
                mapData[y][x] = TILE_CACTUS;
            } else {
                mapData[y][x] = TILE_WATER;
            }
        }
    }
}

void Level::update(float deltaTime) {
    // Update animation timers
    waterAnimationTimer += deltaTime;
    if (waterAnimationTimer >= ANIMATION_SPEED) {
        waterAnimationTimer = 0;
    }
    
    lavaAnimationTimer += deltaTime;
    if (lavaAnimationTimer >= ANIMATION_SPEED * 0.5f) { // Lava animates faster
        lavaAnimationTimer = 0;
    }
}

void Level::draw(Texture2D backgroundTexture, int worldNumber, float dancingCactusTimer) {
    // Clear background to match the original game's background
    ClearBackground(BLACK);
    
    // Calculate offsets to center the map on screen
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int mapPixelWidth = mapWidth * TILE_SIZE;
    int mapPixelHeight = mapHeight * TILE_SIZE;
    
    int offsetX = (screenWidth - mapPixelWidth) / 2;
    int offsetY = (screenHeight - mapPixelHeight) / 2;
    
    // Store offsets for other elements to use
    this->offsetX = offsetX;
    this->offsetY = offsetY;
    
    // Draw the tile grid
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            AssetsManager& assets = AssetsManager::getInstance();
            Rectangle src;
            Color tint = WHITE;
            
            // Choose sprite based on tile type
            switch (mapData[y][x]) {
                case TILE_WALL:
                    // Use desert wall tile from original game
                    src = Rectangle{224, 0, 16, 16}; // Adjust based on your spritesheet
                    break;
                case TILE_CACTUS:
                    // Use cactus tile with animation
                    src = Rectangle{240, 0, 16, 16}; // Adjust based on your spritesheet
                    // Apply dancing animation if timer is active
                    if (dancingCactusTimer > 400) {
                        src.x += 16;
                    }
                    break;
                case TILE_WATER:
                    // Use water tile with animation
                    src = Rectangle{272, 0, 16, 16}; // Adjust based on your spritesheet
                    // Animate water
                    src.x += (int)(waterAnimationTimer / (ANIMATION_SPEED * 0.5)) * 16;
                    break;
                case TILE_LAVA:
                    // Use lava tile with animation
                    src = Rectangle{304, 0, 16, 16}; // Adjust based on your spritesheet
                    // Animate lava
                    src.x += (int)(lavaAnimationTimer / (ANIMATION_SPEED * 0.5)) * 16;
                    break;
                default:
                    // Floor tiles - use desert floor
                    src = Rectangle{208, 0, 16, 16}; // Adjust based on your spritesheet
                    break;
            }
            
            // Draw the tile with offset
            DrawTexturePro(
                assets.spriteSheet,
                src,
                Rectangle{(float)(offsetX + x * TILE_SIZE), (float)(offsetY + y * TILE_SIZE), 
                          (float)TILE_SIZE, (float)TILE_SIZE},
                Vector2{0, 0},
                0.0f,
                tint
            );
        }
    }
}

bool Level::isPassable(float x, float y) const {
    // Convert world coordinates to tile coordinates
    int tileX = (int)(x / TILE_SIZE);
    int tileY = (int)(y / TILE_SIZE);
    
    // Check bounds
    if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapHeight) {
        return false;
    }
    
    // Check tile type
    TileType tile = mapData[tileY][tileX];
    return tile == TILE_FLOOR; // Only floor tiles are passable
}

void Level::addToSpawnQueue(int enemyType, Vector2 position, float delay) {
    SpawnQueueItem item;
    item.enemyType = enemyType;
    item.position = position;
    item.spawnTime = delay;
    spawnQueue.push(item);
}

bool Level::getNextSpawn(int& enemyType, Vector2& position) {
    if (spawnQueue.empty()) {
        return false;
    }
    
    // Get the next spawn from the queue
    SpawnQueueItem item = spawnQueue.top();
    
    // If the spawn time is still positive, put it back with reduced time
    if (item.spawnTime > 0.01f) { // Use a small epsilon for floating point comparison
        return false;
    }
    
    // Pop the item since we're about to spawn it
    spawnQueue.pop();
    
    // Ready to spawn
    enemyType = item.enemyType;
    position = item.position;
    return true;
}