#include "game/level.h"
#include "game/game_defs.h"
#include "game/enemy.h"
#include "game/raylib_wrapper.h" // Añade esta línea
#include "raymath.h" // Add this line
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

Level::Level() : levelNumber(0),
                 mapWidth(MAP_WIDTH),
                 mapHeight(MAP_HEIGHT),
                 playerStart({384.0f, 384.0f}),
                 waterAnimationTimer(0),
                 lavaAnimationTimer(0),
                 dancingCactusTimer(0) // Añade esta línea
{
    // Initialize empty map
    mapData.resize(mapHeight);
    for (auto& row : mapData) {
        row.resize(mapWidth, TILE_FLOOR);
    }
}

Level::~Level()
{
    // Clear any allocated resources
    while (!spawnQueue.empty())
    {
        spawnQueue.pop();
    }
}

bool Level::loadMap(int levelNum)
{
    // Store level number
    levelNumber = levelNum;

    // Clear existing data
    mapData.clear();
    mapData.resize(mapHeight);
    for (auto& row : mapData) {
        row.resize(mapWidth, TILE_FLOOR);
    }
    enemySpawnPoints.clear();
    powerupSpawnPoints.clear();

    while (!spawnQueue.empty())
    {
        spawnQueue.pop();
    }

    // Generate the appropriate level
    generateLevel(levelNum);

    return true;
}

void Level::generateLevel(int levelNum)
{
    // Determine type of level based on level number
    if (levelNum % 4 == 0)
    {
        // Boss level
        int bossType = (levelNum / 4) % 2; // Alternates between 0 (Cowboy) and 1 (Fector)
        generateBossLevel(bossType);
    }
    else if (levelNum % 4 == 3)
    {
        // Shop level before boss
        generateShopLevel();
    }
    else
    {
        // Standard level
        generateStandardLevel(levelNum);
    }
}

void Level::generateStandardLevel(int levelNum)
{
    // Todos los niveles son de tamaño 16x16 como en el original
    mapWidth = 16;
    mapHeight = 16;
    mapData.clear();
    mapData.resize(mapHeight);
    for (auto& row : mapData) {
        row.resize(mapWidth, TILE_FLOOR);
    }

    // Posición inicial del jugador en el centro
    playerStart = {mapWidth * TILE_SIZE / 2.0f, mapHeight * TILE_SIZE / 2.0f};

    // Primero creamos el mapa base con bordes como en el juego original
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            // Bordes exteriores en esquinas (cactos)
            if ((i == 0 || i == 15 || j == 0 || j == 15) && (i <= 6 || i >= 10) && (j <= 6 || j >= 10))
            {
                mapData[i][j] = TILE_CACTUS; // MAP_CACTUS (5) en el original
            }
            // Resto de bordes exteriores
            else if (i == 0 || i == 15 || j == 0 || j == 15)
            {
                // Barrera aleatoria (0 o 1 en el original)
                mapData[i][j] = TILE_WALL;
            }
            // Segunda fila desde el borde
            else if (i == 1 || i == 14 || j == 1 || j == 14)
            {
                mapData[i][j] = TILE_WALL; // MAP_ROCKY1 (2) en el original
            }
            // Centro del mapa
            else
            {
                // 10% de probabilidad de hierba, resto suelo del desierto
                mapData[i][j] = (GetRandomValue(0, 100) < 10) ? TILE_FLOOR : TILE_FLOOR; 
            }
        }
    }

    // Ahora aplicamos las modificaciones específicas para cada nivel
    switch (levelNum)
    {
    case 1:
        // Nivel 1: Añadir las vallas en las 4 esquinas
        // Esquina superior izquierda
        mapData[4][4] = TILE_WALL; // MAP_FENCE (7) en el original
        mapData[4][5] = TILE_WALL;
        mapData[5][4] = TILE_WALL;

        // Esquina superior derecha
        mapData[4][12] = TILE_WALL;
        mapData[4][11] = TILE_WALL;
        mapData[5][12] = TILE_WALL;

        // Esquina inferior izquierda
        mapData[12][4] = TILE_WALL;
        mapData[12][5] = TILE_WALL;
        mapData[11][4] = TILE_WALL;

        // Esquina inferior derecha
        mapData[12][12] = TILE_WALL;
        mapData[12][11] = TILE_WALL;
        mapData[11][12] = TILE_WALL;
        break;

    case 2:
        // Nivel 2: Cruz central y cactos en las esquinas
        mapData[8][4] = TILE_WALL; // MAP_FENCE (7)
        mapData[12][8] = TILE_WALL;
        mapData[8][12] = TILE_WALL;
        mapData[4][8] = TILE_WALL;
        
        // Cactos en las esquinas interiores
        mapData[1][1] = TILE_CACTUS;
        mapData[14][1] = TILE_CACTUS;
        mapData[14][14] = TILE_CACTUS;
        mapData[1][14] = TILE_CACTUS;
        mapData[2][1] = TILE_CACTUS;
        mapData[13][1] = TILE_CACTUS;
        mapData[13][14] = TILE_CACTUS;
        mapData[2][14] = TILE_CACTUS;
        mapData[1][2] = TILE_CACTUS;
        mapData[14][2] = TILE_CACTUS;
        mapData[14][13] = TILE_CACTUS;
        mapData[1][13] = TILE_CACTUS;
        break;

    case 3:
        // Nivel 3: Paredes interiores formando dos rectángulos
        mapData[5][5] = TILE_WALL;
        mapData[6][5] = TILE_WALL;
        mapData[7][5] = TILE_WALL;
        mapData[9][5] = TILE_WALL;
        mapData[10][5] = TILE_WALL;
        mapData[11][5] = TILE_WALL;
        mapData[5][11] = TILE_WALL;
        mapData[6][11] = TILE_WALL;
        mapData[7][11] = TILE_WALL;
        mapData[9][11] = TILE_WALL;
        mapData[10][11] = TILE_WALL;
        mapData[11][11] = TILE_WALL;
        mapData[5][6] = TILE_WALL;
        mapData[5][7] = TILE_WALL;
        mapData[5][9] = TILE_WALL;
        mapData[5][10] = TILE_WALL;
        mapData[11][6] = TILE_WALL;
        mapData[11][7] = TILE_WALL;
        mapData[11][9] = TILE_WALL;
        mapData[11][10] = TILE_WALL;
        break;

    case 4:
    case 8:
        // Niveles 4 y 8: Río horizontal y algunos muros/cactos
        // Reemplazar los cactos de borde por muros aleatorios
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                if (mapData[i][j] == TILE_CACTUS) {
                    mapData[i][j] = TILE_WALL;
                }
            }
        }
        
        // Añadir agua en el medio
        for (int i = 0; i < 16; i++) {
            mapData[i][8] = TILE_WATER;
        }
        
        // Añadir muros y cactos específicos
        mapData[8][4] = TILE_WALL;
        mapData[8][12] = TILE_WALL;
        mapData[9][12] = TILE_WALL;
        mapData[7][12] = TILE_WALL;
        mapData[5][6] = TILE_CACTUS;
        mapData[10][6] = TILE_CACTUS;
        break;

    case 5:
        // Nivel 5: Cactos en las esquinas y bordes
        mapData[1][1] = TILE_CACTUS;
        mapData[14][1] = TILE_CACTUS;
        mapData[14][14] = TILE_CACTUS;
        mapData[1][14] = TILE_CACTUS;
        mapData[2][1] = TILE_CACTUS;
        mapData[13][1] = TILE_CACTUS;
        mapData[13][14] = TILE_CACTUS;
        mapData[2][14] = TILE_CACTUS;
        mapData[1][2] = TILE_CACTUS;
        mapData[14][2] = TILE_CACTUS;
        mapData[14][13] = TILE_CACTUS;
        mapData[1][13] = TILE_CACTUS;
        mapData[3][1] = TILE_CACTUS;
        mapData[13][1] = TILE_CACTUS;
        mapData[13][13] = TILE_CACTUS;
        mapData[1][13] = TILE_CACTUS;
        mapData[1][3] = TILE_CACTUS;
        mapData[13][3] = TILE_CACTUS;
        mapData[12][13] = TILE_CACTUS;
        mapData[3][14] = TILE_CACTUS;
        mapData[3][3] = TILE_CACTUS;
        mapData[13][12] = TILE_CACTUS;
        mapData[13][12] = TILE_CACTUS;
        mapData[3][12] = TILE_CACTUS;
        break;

    case 6:
        // Nivel 6: Obstáculos dispersos
        mapData[4][5] = TILE_WALL;
        mapData[12][10] = TILE_CACTUS;
        mapData[10][9] = TILE_CACTUS;
        mapData[5][12] = TILE_WALL;
        mapData[5][9] = TILE_CACTUS;
        mapData[12][12] = TILE_CACTUS;
        mapData[3][4] = TILE_CACTUS;
        mapData[2][3] = TILE_CACTUS;
        mapData[11][3] = TILE_CACTUS;
        mapData[10][6] = TILE_CACTUS;
        mapData[5][9] = TILE_WALL;
        mapData[10][12] = TILE_WALL;
        mapData[3][12] = TILE_WALL;
        mapData[10][8] = TILE_WALL;
        break;

    case 7:
        // Nivel 7: Dos ríos horizontales con puentes
        for (int i = 0; i < 16; i++) {
            mapData[i][5] = TILE_WATER;
            mapData[i][10] = TILE_WATER;
        }
        // Puentes
        mapData[4][5] = TILE_LAVA; // MAP_BRIDGE (10)
        mapData[8][5] = TILE_LAVA;
        mapData[12][5] = TILE_LAVA;
        mapData[4][10] = TILE_LAVA;
        mapData[8][10] = TILE_LAVA;
        mapData[12][10] = TILE_LAVA;
        break;

    case 9:
        // Nivel 9: Mucho cactos en patrón específico
        mapData[4][4] = TILE_CACTUS;
        mapData[5][4] = TILE_CACTUS;
        mapData[10][4] = TILE_CACTUS;
        mapData[12][4] = TILE_CACTUS;
        mapData[4][5] = TILE_CACTUS;
        mapData[5][5] = TILE_CACTUS;
        mapData[10][5] = TILE_CACTUS;
        mapData[12][5] = TILE_CACTUS;
        mapData[4][10] = TILE_CACTUS;
        mapData[5][10] = TILE_CACTUS;
        mapData[10][10] = TILE_CACTUS;
        mapData[12][10] = TILE_CACTUS;
        mapData[4][12] = TILE_CACTUS;
        mapData[5][12] = TILE_CACTUS;
        mapData[10][12] = TILE_CACTUS;
        mapData[12][12] = TILE_CACTUS;
        break;

    case 10:
        // Nivel 10: Ríos horizontales con puentes y cactos
        for (int i = 0; i < 16; i++) {
            mapData[i][1] = TILE_WATER;
            mapData[i][14] = TILE_WATER;
        }
        // Puentes
        mapData[8][1] = TILE_LAVA; // MAP_BRIDGE (10)
        mapData[7][1] = TILE_LAVA;
        mapData[9][1] = TILE_LAVA;
        mapData[8][14] = TILE_LAVA;
        mapData[7][14] = TILE_LAVA;
        mapData[9][14] = TILE_LAVA;
        // Cactos
        mapData[6][8] = TILE_CACTUS;
        mapData[10][8] = TILE_CACTUS;
        mapData[8][6] = TILE_CACTUS;
        mapData[8][9] = TILE_CACTUS;
        break;

    case 11:
        // Nivel 11: Muros horizontales y cactos
        for (int i = 0; i < 16; i++) {
            mapData[i][0] = TILE_WALL; // MAP_FENCE (7)
            mapData[i][15] = TILE_WALL;
            if (i % 2 == 0) {
                mapData[i][1] = TILE_CACTUS;
                mapData[i][14] = TILE_CACTUS;
            }
        }
        break;

    case 12:
        // Nivel 12: Layout para el jefe final
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                if (mapData[i][j] == TILE_WALL && (i == 0 || i == 15 || j == 0 || j == 15)) {
                    mapData[i][j] = TILE_CACTUS;
                }
            }
        }
        
        // Ríos en los bordes superior e inferior
        for (int i = 0; i < 16; i++) {
            mapData[i][0] = TILE_WATER;
            mapData[i][15] = TILE_WATER;
        }
        
        // Bordes internos
        for (int i = 1; i <= 14; i++) {
            for (int j = 1; j <= 14; j++) {
                if (i == 1 || i == 14 || j == 1 || j == 14) {
                    mapData[i][j] = TILE_LAVA; // MAP_BRIDGE (10)
                } else if (i == 2 || i == 13 || j == 2 || j == 13) {
                    mapData[i][j] = TILE_WALL; // MAP_ROCKY1 (2)
                }
            }
        }
        break;

    default:
        // Niveles no definidos: cactos en las esquinas
        mapData[4][4] = TILE_CACTUS;
        mapData[12][4] = TILE_CACTUS;
        mapData[4][12] = TILE_CACTUS;
        mapData[12][12] = TILE_CACTUS;
        break;
    }

    // Configurar puntos de aparición para enemigos
    enemySpawnPoints.clear();
    enemySpawnPoints.push_back(Vector2{TILE_SIZE * 1.5f, TILE_SIZE * 1.5f});                               // Superior izquierda
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE - TILE_SIZE * 1.5f, TILE_SIZE * 1.5f});        // Superior derecha
    enemySpawnPoints.push_back(Vector2{TILE_SIZE * 1.5f, mapHeight * TILE_SIZE - TILE_SIZE * 1.5f});       // Inferior izquierda
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE - TILE_SIZE * 1.5f, mapHeight * TILE_SIZE - TILE_SIZE * 1.5f}); // Inferior derecha

    // Configurar puntos de aparición para powerups
    powerupSpawnPoints.clear();
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 3.0f, mapHeight * TILE_SIZE / 3.0f});
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE * 2.0f / 3.0f, mapHeight * TILE_SIZE / 3.0f});
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 3.0f, mapHeight * TILE_SIZE * 2.0f / 3.0f});
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE * 2.0f / 3.0f, mapHeight * TILE_SIZE * 2.0f / 3.0f});
}

void Level::setupSpawnPoints()
{
    // Clear existing spawn points
    enemySpawnPoints.clear();
    powerupSpawnPoints.clear();

    // Add enemy spawn points at each corner with an offset from the wall
    enemySpawnPoints.push_back(Vector2{TILE_SIZE * 2.0f, TILE_SIZE * 2.0f});                                                // Top-left
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE - TILE_SIZE * 3.0f, TILE_SIZE * 2.0f});                         // Top-right
    enemySpawnPoints.push_back(Vector2{TILE_SIZE * 2.0f, mapHeight * TILE_SIZE - TILE_SIZE * 3.0f});                        // Bottom-left
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE - TILE_SIZE * 3.0f, mapHeight * TILE_SIZE - TILE_SIZE * 3.0f}); // Bottom-right

    // Add middle spawn points on each edge
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 2.0f, TILE_SIZE * 2.0f});                         // Top-middle
    enemySpawnPoints.push_back(Vector2{TILE_SIZE * 2.0f, mapHeight * TILE_SIZE / 2.0f});                        // Left-middle
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE - TILE_SIZE * 3.0f, mapHeight * TILE_SIZE / 2.0f}); // Right-middle
    enemySpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 2.0f, mapHeight * TILE_SIZE - TILE_SIZE * 3.0f}); // Bottom-middle

    // Set powerup spawn locations (similar to original game positions)
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 3.0f, mapHeight * TILE_SIZE / 3.0f});
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE * 2.0f / 3.0f, mapHeight * TILE_SIZE / 3.0f});
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE / 3.0f, mapHeight * TILE_SIZE * 2.0f / 3.0f});
    powerupSpawnPoints.push_back(Vector2{mapWidth * TILE_SIZE * 2.0f / 3.0f, mapHeight * TILE_SIZE * 2.0f / 3.0f});
}

void Level::generateBossLevel(int bossType)
{
    // Set player start position
    playerStart.x = static_cast<float>(mapWidth * TILE_SIZE) / 2.0f;
    playerStart.y = static_cast<float>(mapHeight * TILE_SIZE) * 3.0f / 4.0f;

    // Boss levels have simpler layouts with fewer obstacles
    for (int y = 0; y < mapHeight; y++)
    {
        for (int x = 0; x < mapWidth; x++)
        {
            if (x == 0 || y == 0 || x == mapWidth - 1 || y == mapHeight - 1)
            {
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
    addToSpawnQueue(bossEnemyType, bossSpawn, 2.0f); // Spawn boss after 2 seconds

    // Add some minions
    int minions = 6 + bossType * 2; // 6 for cowboy, 8 for fector

    for (int i = 0; i < minions; i++)
    {
        // Random position around the boss
        float angle = (float)i / minions * 2.0f * PI;
        Vector2 minionPos = {
            bossSpawn.x + cosf(angle) * TILE_SIZE * 3.0f,
            bossSpawn.y + sinf(angle) * TILE_SIZE * 3.0f};

        // Choose appropriate minion type
        int minionType = (bossType == 0) ? ENEMY_ORC : ENEMY_GHOST;

        // Add to spawn queue with increasing delays
        addToSpawnQueue(minionType, minionPos, 4.0f + i * 0.5f);
    }
}

void Level::generateShopLevel(void)
{
    // Set player start position
    playerStart = {mapWidth * TILE_SIZE / 2.0f, mapHeight * TILE_SIZE * 3.0f / 4.0f};

    // Shop levels have walls that create a small enclosed area
    for (int y = 0; y < mapHeight; y++)
    {
        for (int x = 0; x < mapWidth; x++)
        {
            // Create outer walls
            if (x == 0 || y == 0 || x == mapWidth - 1 || y == mapHeight - 1)
            {
                mapData[y][x] = TILE_WALL;
            }

            // Create inner shop area
            if (x >= mapWidth / 3 && x <= mapWidth * 2 / 3 &&
                y >= mapHeight / 3 && y <= mapHeight * 2 / 3)
            {

                // Make interior floor
                mapData[y][x] = TILE_FLOOR;

                // Make walls around the shop
                if (x == mapWidth / 3 || x == mapWidth * 2 / 3 ||
                    y == mapHeight / 3 || y == mapHeight * 2 / 3)
                {

                    mapData[y][x] = TILE_WALL;

                    // Create entrance at bottom
                    if (y == mapHeight * 2 / 3 && x >= mapWidth / 3 + 2 && x <= mapWidth * 2 / 3 - 2)
                    {
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

void Level::addObstacles(int count)
{
    // Add random obstacles in valid locations
    for (int i = 0; i < count; i++)
    {
        int attempts = 0;
        int x, y;
        float dist;

        // Try to find a valid position
        while (attempts < 100)
        {
            // Generate random position (avoid borders)
            x = GetRandomValue(2, mapWidth - 3);
            y = GetRandomValue(2, mapHeight - 3);

            // Calculate distance to player start
            dist = Vector2Distance(Vector2{(float)x * TILE_SIZE, (float)y * TILE_SIZE}, playerStart);

            // If this is a floor tile and far enough from player start
            if (mapData[y][x] == TILE_FLOOR && dist > TILE_SIZE * 3)
            {
                break;
            }

            attempts++;
        }

        // If a valid position was found, place obstacle
        if (attempts < 100)
        {
            // Choose obstacle type
            int obstacleType = GetRandomValue(0, 10);
            if (obstacleType < 7)
            {
                mapData[y][x] = TILE_WALL;
            }
            else if (obstacleType < 9)
            {
                mapData[y][x] = TILE_CACTUS;
            }
            else
            {
                mapData[y][x] = TILE_WATER;
            }
        }
    }
}

void Level::update(float deltaTime)
{
    // Update animation timers
    waterAnimationTimer += deltaTime;
    if (waterAnimationTimer >= ANIMATION_SPEED)
    {
        waterAnimationTimer = 0;
    }

    lavaAnimationTimer += deltaTime;
    if (lavaAnimationTimer >= ANIMATION_SPEED * 0.5f)
    { 
        lavaAnimationTimer = 0;
    }
    
    // Actualiza el timer para cactos bailarines
    dancingCactusTimer += deltaTime * 1000.0f; // Multiplicamos por 1000 para convertir a ms
    if (dancingCactusTimer >= 1600.0f) { // Ciclo completo de 1.6 segundos
        dancingCactusTimer = 0;
    }

    // Example usage of Vector2Distance
    float distance = Vector2Distance(playerStart, {0, 0});
}

void Level::draw(Texture2D backgroundTexture, int worldNumber) {
    // Get offsets for centering
    Vector2 gameOffset = AssetsManager::GetGameOffset();
    offsetX = gameOffset.x;
    offsetY = gameOffset.y;
    float zoom = AssetsManager::GetZoom();
    
    // For each tile in the map
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            float drawX = x * TILE_SIZE + offsetX;
            float drawY = y * TILE_SIZE + offsetY;
            
            // Draw the tile based on its type
            TileType tile = mapData[y][x];
            Rectangle srcRect = GetTileSourceRect(tile, worldNumber);
            
            DrawTexturePro(
                backgroundTexture,
                srcRect,
                Rectangle{drawX, drawY, TILE_SIZE, TILE_SIZE},
                Vector2{0, 0},
                0.0f,
                WHITE
            );
        }
    }
}

bool Level::isPassable(float x, float y) const
{
    // Convert world coordinates to tile coordinates
    int tileX = (int)(x / TILE_SIZE);
    int tileY = (int)(y / TILE_SIZE);

    // Check bounds
    if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapHeight)
    {
        return false;
    }

    // Check tile type
    TileType tile = mapData[tileY][tileX];
    return tile == TILE_FLOOR; // Only floor tiles are passable
}

void Level::addToSpawnQueue(int enemyType, Vector2 position, float delay)
{
    SpawnQueueItem item;
    item.enemyType = enemyType;
    item.position = position;
    item.spawnTime = delay;
    spawnQueue.push(item);
}

bool Level::getNextSpawn(int &enemyType, Vector2 &position)
{
    if (spawnQueue.empty())
    {
        return false;
    }

    // Get the next spawn from the queue
    SpawnQueueItem item = spawnQueue.top();

    // If the spawn time is still positive, put it back with reduced time
    if (item.spawnTime > 0.01f)
    { // Use a small epsilon for floating point comparison
        return false;
    }

    // Pop the item since we're about to spawn it
    spawnQueue.pop();

    // Ready to spawn
    enemyType = item.enemyType;
    position = item.position;
    return true;
}

Rectangle Level::GetTileSourceRect(TileType tile, int worldNumber) {
    // Default position for floor tile
    int x = 464;
    int y = 1680 - (worldNumber * 16);

    switch (tile) {
        case TILE_FLOOR:
            // Floor tile (default)
            break;
        case TILE_WALL:
            x = 464 + 16; // Next tile is wall
            break;
        case TILE_WATER:
            x = 464 + 32; // Water tile
            // Add animation offset based on water animation timer
            if (this->waterAnimationTimer > ANIMATION_SPEED / 2.0f) {
                x += 16;
            }
            break;
        case TILE_CACTUS:
            x = 464 + 64; // Cactus tile
            // Add dancing cactus effect
            if (this->dancingCactusTimer > 800.0f) {
                x += 16;
            }
            break;
        case TILE_LAVA:
            x = 464 + 80; // Lava/Bridge tile
            // Add animation effect
            if (this->lavaAnimationTimer > ANIMATION_SPEED * 0.25f) {
                x += 16;
            }
            break;
    }

    // Adjust for sprite sheet offset
    x -= 128;
    y -= 1648;
    
    // Asegúrate de devolver el rectángulo
    return Rectangle{(float)x, (float)y, 16.0f, 16.0f};
}