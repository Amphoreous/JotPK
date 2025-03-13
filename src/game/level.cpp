#include "game/level.h"
#include "game/game_defs.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

#define ANIMATION_SPEED 0.2f


Level::Level() : levelNumber(0), 
                 waveTimer(WAVE_DURATION),
                 mapWidth(MAP_WIDTH), 
                 mapHeight(MAP_HEIGHT),
                 playerStart({384.0f, 384.0f}),
                 waterAnimationTimer(0),
                 lavaAnimationTimer(0),
                 dancingCactusTimer(0),
                 offsetX(0),
                 offsetY(0) {
    initializeMap();
}

Level::~Level() {
    clearSpawnQueue();
}

void Level::initializeMap() {
    mapData.resize(mapHeight);
    for (auto& row : mapData)
        row.resize(mapWidth, MAP_DESERT);
}

bool Level::loadMap(int waveNumber) {
    levelNumber = waveNumber;
    initializeMap();
    generateWaveMap(waveNumber);
    setupWaveSpawns(waveNumber);
    return true;
}

void Level::generateWaveMap(int wave) {
    // Base layer - same as original
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if ((x == 0 || x == 15 || y == 0 || y == 15) && 
               (x <= 6 || x >= 10) && (y <= 6 || y >= 10)) {
                mapData[y][x] = MAP_CACTUS;
            } else if (x == 0 || x == 15 || y == 0 || y == 15) {
                mapData[y][x] = (GetRandomValue(0, 100) < 15) ? MAP_BARRIER1 : MAP_BARRIER2;
            } else if (x == 1 || x == 14 || y == 1 || y == 14) {
                mapData[y][x] = MAP_ROCKY1;
            } else {
                mapData[y][x] = (GetRandomValue(0, 100) < 10) ? MAP_GRASSY : MAP_DESERT;
            }
        }
    }

    // Wave-specific layouts
    switch (wave) {
        case -1: // Victory map
            createVictoryMap();
            break;

        case 1: // Wave 1 - corners blocked with fences
            mapData[4][4] = MAP_FENCE;
            mapData[4][5] = MAP_FENCE;
            mapData[5][4] = MAP_FENCE;
            mapData[12][4] = MAP_FENCE;
            mapData[11][4] = MAP_FENCE; 
            mapData[12][5] = MAP_FENCE;
            mapData[4][12] = MAP_FENCE;
            mapData[5][12] = MAP_FENCE;
            mapData[4][11] = MAP_FENCE;
            mapData[12][12] = MAP_FENCE;
            mapData[11][12] = MAP_FENCE;
            mapData[12][11] = MAP_FENCE;
            break;

        case 2: // Wave 2 - central cross pattern
            mapData[8][4] = MAP_FENCE;
            mapData[12][8] = MAP_FENCE;
            mapData[8][12] = MAP_FENCE;
            mapData[4][8] = MAP_FENCE;
            mapData[1][1] = MAP_CACTUS;
            mapData[14][1] = MAP_CACTUS;
            mapData[14][14] = MAP_CACTUS;
            mapData[1][14] = MAP_CACTUS;
            break;

        case 3: // Wave 3 - dual rectangles
            for (int x = 5; x <= 11; x++) {
                mapData[5][x] = MAP_FENCE;
                mapData[11][x] = MAP_FENCE;
            }
            for (int y = 6; y <= 10; y++) {
                mapData[y][5] = MAP_FENCE;
                mapData[y][11] = MAP_FENCE;
            }
            break;

        case 4:
        case 8: // Shop waves
            generateShopLayout();
            for (int i = 0; i < 16; i++) {
                mapData[i][8] = (GetRandomValue(0, 1) == 0) ? MAP_TRENCH1 : MAP_TRENCH2;
            }
            break;

        case 12: // Boss arena
            createBossArena();
            break;

        default:
            break;
    }
}

void Level::createVictoryMap() {
    // Clear central area
    for (int y = 3; y < 13; y++) {
        for (int x = 3; x < 13; x++) {
            mapData[y][x] = MAP_DESERT;
        }
    }
    
    // Add victory map decorations
    mapData[3][1] = MAP_CACTUS;
    mapData[8][2] = MAP_CACTUS;
    mapData[12][3] = MAP_CACTUS;
    
    // Place celebratory fences
    for (int x = 5; x < 11; x++) {
        mapData[5][x] = MAP_FENCE;
        mapData[10][x] = MAP_FENCE;
    }
}

void Level::createBossArena() {
    // Clear central area for boss fight
    for (int y = 4; y < 12; y++) {
        for (int x = 4; x < 12; x++) {
            mapData[y][x] = MAP_DESERT;
        }
    }
    
    // Add arena barriers
    for (int x = 3; x < 13; x++) {
        mapData[3][x] = MAP_BARRIER1;
        mapData[12][x] = MAP_BARRIER1;
    }
    
    // Add trenches for cover
    mapData[6][5] = MAP_TRENCH1;
    mapData[6][10] = MAP_TRENCH1;
    mapData[9][5] = MAP_TRENCH2;
    mapData[9][10] = MAP_TRENCH2;
}

void Level::generateShopLayout() {
    // Create merchant area
    for (int y = 5; y < 11; y++) {
        for (int x = 5; x < 11; x++) {
            mapData[y][x] = MAP_DESERT;
        }
    }
    
    // Add shop walls
    for (int x = 5; x < 11; x++) {
        mapData[5][x] = MAP_FENCE;
        if (x != 7 && x != 8) {  // Leave entrance open
            mapData[10][x] = MAP_FENCE;
        }
    }
    
    // Add shop corners
    mapData[5][5] = MAP_BARRIER1;
    mapData[5][10] = MAP_BARRIER1;
    mapData[10][5] = MAP_BARRIER1;
    mapData[10][10] = MAP_BARRIER1;
}

void Level::addWaveSpecificFeatures(int wave) {
    int obstacleCount = 10 + wave * 2;
    
    for (int i = 0; i < obstacleCount; i++) {
        int x = GetRandomValue(2, mapWidth-3);
        int y = GetRandomValue(2, mapHeight-3);
        
        if (mapData[y][x] == MAP_DESERT) {
            switch(GetRandomValue(0, 2)) {
                case 0: mapData[y][x] = MAP_ROCKY1; break;
                case 1: mapData[y][x] = MAP_TRENCH1; break;
                case 2: mapData[y][x] = MAP_BRIDGE; break;
            }
        }
    }
}

void Level::setupWaveSpawns(int wave) {
    enemySpawnPoints.clear();
    powerupSpawnPoints.clear();

    // Base spawn points in corners matching original game
    const Vector2 spawns[] = {
        {TILE_SIZE * 1.5f, TILE_SIZE * 1.5f},
        {TILE_SIZE * 14.5f, TILE_SIZE * 1.5f},
        {TILE_SIZE * 1.5f, TILE_SIZE * 14.5f},
        {TILE_SIZE * 14.5f, TILE_SIZE * 14.5f}
    };

    // Add corner spawns
    for (const auto& spawn : spawns) {
        enemySpawnPoints.push_back(spawn);
    }

    // Wave-specific spawns
    if (wave == 12) { // Boss wave
        addToSpawnQueue(ENEMY_BOSS_COWBOY, {TILE_SIZE * 8, TILE_SIZE * 3}, 0);
    } else if (wave % 4 == 0) { // Mini-boss waves
        addToSpawnQueue(ENEMY_MINI_BOSS, {TILE_SIZE * 8, TILE_SIZE * 13}, 2000);
    }

    // Add random enemies based on wave number
    int baseEnemies = 5 + wave * 2;
    for (int i = 0; i < baseEnemies; i++) {
        Vector2 spawnPos = enemySpawnPoints[GetRandomValue(0, enemySpawnPoints.size()-1)];
        EnemyType type = static_cast<EnemyType>(GetRandomValue(0, ENEMY_TYPES-1));
        addToSpawnQueue(type, spawnPos, GetRandomValue(0, WAVE_DURATION));
    }
}

void Level::addToSpawnQueue(int enemyType, Vector2 position, float delay) {
    SpawnQueueItem item;
    item.enemyType = enemyType;
    item.position = position;
    item.spawnTime = GetTime() + delay/1000.0f;
    spawnQueue.push(item);
}

void Level::clearSpawnQueue() {
    while (!spawnQueue.empty()) {
        spawnQueue.pop();
    }
}

void Level::update(float deltaTime) {
    // Update animation timers
    waterAnimationTimer += deltaTime;
    if (waterAnimationTimer >= 0.2f) 
        waterAnimationTimer = 0;
    
    lavaAnimationTimer += deltaTime;
    if (lavaAnimationTimer >= 0.15f) 
        lavaAnimationTimer = 0;
    
    // Fix: Change cactus animation timing to match old code
    dancingCactusTimer += deltaTime * 1000.0f;  // Convert to milliseconds
    if (dancingCactusTimer >= 1600.0f)  // Changed from 0.5f to 1600.0f
        dancingCactusTimer = 0;
        
    // Update wave timer
    if (waveTimer > 0)
        waveTimer -= deltaTime * 1000.0f;
}

bool Level::getNextSpawn(int& enemyType, Vector2& position) {
    if (spawnQueue.empty()) {
        return false;
    }
    
    if (spawnQueue.top().spawnTime <= GetTime()) {
        enemyType = spawnQueue.top().enemyType;
        position = spawnQueue.top().position;
        spawnQueue.pop();
        return true;
    }
    
    return false;
}

void Level::draw(Texture2D tileset, int worldTheme) {
    Vector2 gameOffset = AssetsManager::GetGameOffset();
    float zoom = AssetsManager::GetZoom();
    
    // Calculate max zoom to keep map within screen bounds
    float maxZoomX = (float)GetScreenWidth() / (mapWidth * TILE_SIZE);
    float maxZoomY = (float)GetScreenHeight() / (mapHeight * TILE_SIZE);
    float maxZoom = (maxZoomX < maxZoomY) ? maxZoomX : maxZoomY;
    if (zoom > maxZoom)
        zoom = maxZoom;

    // Calculate center offset
    float totalMapWidth = mapWidth * TILE_SIZE * zoom;
    float totalMapHeight = mapHeight * TILE_SIZE * zoom;
    offsetX = (GetScreenWidth() - totalMapWidth) / 2.0f;
    offsetY = (GetScreenHeight() - totalMapHeight) / 2.0f;

    // Draw all tiles
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            float drawX = x * TILE_SIZE * zoom + offsetX;
            float drawY = y * TILE_SIZE * zoom + offsetY;
            
            int tile = mapData[y][x];
            Rectangle srcRect = GetTileSourceRect(static_cast<TileType>(tile), worldTheme);
            
            DrawTexturePro(
                tileset,
                srcRect,
                Rectangle{ drawX, drawY, TILE_SIZE * zoom, TILE_SIZE * zoom },
                Vector2{ 0, 0 },
                0.0f,
                WHITE
            );
        }
    }
}

Rectangle Level::GetTileSourceRect(TileType tile, int worldNumber) {
    int x = 0;
    int y = 32 - (worldNumber * 16);
    
    switch (tile) {
        case MAP_BARRIER1:
            x = 464 - 128;           // 336
            break;
        case MAP_BARRIER2:
            x = (464 + 16) - 128;    // 352
            break;
        case MAP_ROCKY1:
            x = (464 + 32) - 128;    // 368
            break;
        case MAP_DESERT:
            x = (464 + 48) - 128;    // 384
            break;
        case MAP_GRASSY:
            x = (464 + 64) - 128;    // 400
            break;
        case MAP_CACTUS:
            x = (464 + 80) - 128;    // 416
            if (dancingCactusTimer > 800.0f)  // Changed from 0.5f to 800.0f
                x += 16;             // 432
            break;
        case MAP_FENCE:
            x = (464 + 112) - 128;   // 448
            break;
        case MAP_TRENCH1:
            x = (464 + 128) - 128;   // 464
            if (waterAnimationTimer > ANIMATION_SPEED / 2.0f)
                x += 16;             // 480
            break;
        case MAP_TRENCH2:
            x = (464 + 144) - 128;   // 480
            if (waterAnimationTimer > ANIMATION_SPEED / 2.0f)
                x += 16;             // 496
            break;
        case MAP_BRIDGE:
            x = (464 + 160) - 128;   // 496
            if (lavaAnimationTimer > ANIMATION_SPEED * 0.25f)
                x += 16;             // 512
            break;
        default:
            x = (464 + 48) - 128;    // 384 (MAP_DESERT)
            break;
    }

    return Rectangle{ (float)x, (float)y, 16.0f, 16.0f };
}

bool Level::isPassable(TileType tileType) const {
    // Based on AbigailGame.cs isMapTilePassable
    if (tileType <= MAP_BARRIER2 || 
        (tileType >= MAP_CACTUS && tileType <= MAP_BRIDGE)) {
        return false;
    }
    return true;
}

bool Level::isPassableForMonsters(TileType tileType) {
    // Based on AbigailGame.cs isMapTilePassableForMonsters
    if (tileType == MAP_CACTUS || 
        (tileType >= MAP_FENCE && tileType <= MAP_TRENCH2)) {
        return false;
    }
    return true;
}

bool Level::isPassable(float worldX, float worldY) const {
    // Convert world coordinates to tile coordinates
    int tileX = static_cast<int>(worldX / TILE_SIZE); 
    int tileY = static_cast<int>(worldY / TILE_SIZE);
    
    // Check bounds
    if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapHeight) {
        return false;
    }
    
    // Get tile type at coordinates
    int tileType = mapData[tileY][tileX];
    
    // Use existing tile passability check
    return isPassable(static_cast<TileType>(tileType));
}