#include "game/level.h"
#include "game/game_defs.h"
#include "game/enemy.h"
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
    playerStart = MakeVector2(mapWidth * TILE_SIZE / 2.0f, mapHeight * TILE_SIZE / 2.0f);
    
    // Create border walls
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (x == 0 || y == 0 || x == mapWidth - 1 || y == mapHeight - 1) {
                mapData[y][x] = TILE_WALL;
            }
        }
    }
    
    // Determine number of obstacles based on level
    int obstacleCount = std::min(5 + levelNum, MAX_OBSTACLES);
    
    // Add random obstacles
    addObstacles(obstacleCount);
    
    // Add enemy spawn points (usually near the corners and edges)
    enemySpawnPoints.push_back(MakeVector2((float)TILE_SIZE, (float)TILE_SIZE));                         // Top-left
    enemySpawnPoints.push_back(MakeVector2(mapWidth * TILE_SIZE - (float)TILE_SIZE, (float)TILE_SIZE));  // Top-right
    enemySpawnPoints.push_back(MakeVector2((float)TILE_SIZE, mapHeight * TILE_SIZE - (float)TILE_SIZE)); // Bottom-left
    enemySpawnPoints.push_back(MakeVector2(mapWidth * TILE_SIZE - (float)TILE_SIZE, mapHeight * TILE_SIZE - (float)TILE_SIZE)); // Bottom-right
    
    // Add some mid-edge spawn points
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 2.0f, TILE_SIZE * 2.0f}); // Top-middle
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 2.0f, mapHeight * TILE_SIZE - TILE_SIZE * 2.0f}); // Bottom-middle
    enemySpawnPoints.push_back(Vector2{TILE_SIZE * 2.0f, mapHeight * TILE_SIZE / 2.0f}); // Left-middle
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE - TILE_SIZE * 2.0f, mapHeight * TILE_SIZE / 2.0f}); // Right-middle
    
    // Add powerup spawn points (usually more central)
    powerupSpawnPoints.push_back({mapWidth * TILE_SIZE / 4.0f, mapHeight * TILE_SIZE / 4.0f});
    powerupSpawnPoints.push_back({mapWidth * TILE_SIZE * 3.0f / 4.0f, mapHeight * TILE_SIZE / 4.0f});
    powerupSpawnPoints.push_back({mapWidth * TILE_SIZE / 4.0f, mapHeight * TILE_SIZE * 3.0f / 4.0f});
    powerupSpawnPoints.push_back({mapWidth * TILE_SIZE * 3.0f / 4.0f, mapHeight * TILE_SIZE * 3.0f / 4.0f});
    
    // Queue initial enemies based on level number
    int enemiesToSpawn = 5 + std::min(levelNum * 2, 20);  // More enemies as levels progress
    
    // Distribute enemies among spawn points with random delays
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> delayDist(0.0f, 5.0f);  // 0-5 second delay
    
    for (int i = 0; i < enemiesToSpawn; i++) {
        // Select a random spawn point
        int spawnPointIdx = i % enemySpawnPoints.size();
        Vector2 spawnPos = enemySpawnPoints[spawnPointIdx];
        
        // Determine enemy type based on level
        int enemyType = ENEMY_ORC;  // Default
        
        // As level progresses, introduce harder enemies
        if (levelNum >= 8) {
            enemyType = GetRandomValue(0, 6);  // All enemy types
        } else if (levelNum >= 6) {
            enemyType = GetRandomValue(0, 5);  // Up to Devil
        } else if (levelNum >= 4) {
            enemyType = GetRandomValue(0, 3);  // Up to Mummy
        } else if (levelNum >= 2) {
            enemyType = GetRandomValue(0, 1);  // Orc or Ghost
        }
        
        // Add to spawn queue with random delay
        float delay = delayDist(gen);
        addToSpawnQueue(enemyType, spawnPos, delay);
    }
}

// Fix conversions around lines 275-283

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
    int bossEnemyType = (bossType == 0) ? ENEMY_BOSS_COWBOY : ENEMY_BOSS_FECTOR;
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
        while (attempts < 100) { // Avoid infinite loop
            int x = GetRandomValue(2, mapWidth - 3);
            int y = GetRandomValue(2, mapHeight - 3);
            
            // Check if position is valid (not too close to player start)
            float dist = sqrtf(powf(x*TILE_SIZE - playerStart.x, 2) + powf(y*TILE_SIZE - playerStart.y, 2));
            if (mapData[y][x] == TILE_FLOOR && dist > TILE_SIZE * 3) {
                // Choose obstacle type
                TileType obstacleType = (GetRandomValue(0, 100) < 70) ? TILE_WALL : 
                                      (GetRandomValue(0, 100) < 50) ? TILE_CACTUS : TILE_WATER;
                
                mapData[y][x] = obstacleType;
                break;
            }
            attempts++;
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
    // Draw background
    ClearBackground(DARKBROWN);
    
    if (backgroundTexture.id != 0) {
        // Draw tiled background
        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                // Select source rectangle based on world number and tile type
                Rectangle source = {0, 0, TILE_SIZE, TILE_SIZE};
                
                // Modify source based on world theme (row in spritesheet)
                source.y = worldNumber * TILE_SIZE;
                
                // For animated tiles
                if (mapData[y][x] == TILE_WATER) {
                    int frame = (int)(waterAnimationTimer / ANIMATION_SPEED * 4);
                    source.x = (2 + frame) * TILE_SIZE;  // Water animation frames
                } else if (mapData[y][x] == TILE_LAVA) {
                    int frame = (int)(lavaAnimationTimer / (ANIMATION_SPEED * 0.5f) * 4);
                    source.x = (6 + frame) * TILE_SIZE;  // Lava animation frames
                } else if (mapData[y][x] == TILE_CACTUS) {
                    // Dancing cactus effect
                    float sway = sinf(dancingCactusTimer * 5.0f) * 3.0f;
                    
                    DrawTexturePro(
                        backgroundTexture,
                        Rectangle{TILE_SIZE, source.y, TILE_SIZE, TILE_SIZE},
                        Rectangle{static_cast<float>(x*TILE_SIZE), static_cast<float>(y*TILE_SIZE), static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)},
                        Vector2{TILE_SIZE/2.0f, TILE_SIZE},
                        sway,
                        WHITE
                    );
                    continue;  // Skip normal drawing
                } else if (mapData[y][x] == TILE_WALL) {
                    source.x = 0;  // Wall tile
                } else {
                    source.x = 0;  // Default floor tile
                }
                
                // Draw the tile
                DrawTextureRec(backgroundTexture, source, Vector2{static_cast<float>(x*TILE_SIZE), static_cast<float>(y*TILE_SIZE)}, WHITE);
            }
        }
    } else {
        // Fallback if texture is not loaded - draw colored rectangles
        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                Color tileColor;
                
                switch (mapData[y][x]) {
                    case TILE_WALL:   tileColor = DARKGRAY; break;
                    case TILE_CACTUS: tileColor = GREEN; break;
                    case TILE_WATER:  tileColor = BLUE; break;
                    case TILE_LAVA:   tileColor = RED; break;
                    default:         tileColor = BEIGE; break; // Floor
                }
                
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileColor);
            }
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
    spawnQueue.pop();
    
    // If the spawn time is still positive, put it back with reduced time
    if (item.spawnTime > 0) {
        item.spawnTime -= 0.016f;  // Assume 60fps, 16ms per frame
        spawnQueue.push(item);
        return false;
    }
    
    // Ready to spawn
    enemyType = item.enemyType;
    position = item.position;
    return true;
}