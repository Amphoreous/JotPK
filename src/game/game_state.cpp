#include "raylib.h"
#include "game/game_state.h"
#include "game/game_defs.h"  // Include the new header with definitions
#include "game/assets_manager.h"
#include "resource_manager.h"  // Include resource manager
#include <algorithm>
#include <cstdlib>
#include <iostream>

#define WAVE_DURATION 80000.0f
#define BETWEEN_WAVE_DURATION 5000.0f
#define DEATH_DURATION 3000.0f
#define PLAYER_INVINCIBILITY_DURATION 5000.0f

GameStateManager::GameStateManager() {
    // Initialize default values
    currentState = START_MENU;
    whichWave = 1;  // Cambiado de 0 a 1
    world = 0;
    whichRound = 0;
    lives = 3;
    coins = 0;
    score = 0;
    died = false;
    
    waveTimer = WAVE_DURATION;
    betweenWaveTimer = BETWEEN_WAVE_DURATION;
    deathTimer = 0.0f;
    shopTimer = 0.0f;
    playerInvincibilityTimer = 0.0f;
    
    heldItemType = -1;  // No item held
    
    // Initialize monster spawn chances (matching original game)
    monsterChances = {
        {0.014f, 0.4f},   // Orc
        {0.0f, 0.0f},     // Ghost
        {0.0f, 0.0f},     // Ogre
        {0.0f, 0.0f},     // Mummy
        {0.0f, 0.0f},     // Devil
        {0.0f, 0.0f},     // Mushroom
        {0.0f, 0.0f}      // Spikey
    };
}

GameStateManager::~GameStateManager() {
    // Clean up dynamically allocated objects
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    
    for (Powerup* powerup : powerups) {
        delete powerup;
    }
    powerups.clear();
}

void GameStateManager::Initialize() {
    std::cout << "Initializing game state..." << std::endl;
    
    // Setup initial game state - directly set to PLAYING for testing
    currentState = PLAYING;
    whichWave = 0;  // Change from 0 to 1
    world = 0;
    lives = 3;
    coins = 0;
    score = 0;
    
    // Initialize level first
    std::cout << "Loading level for wave " << whichWave << std::endl;
    level.loadMap(whichWave);
    
    // Set player position based on level's start position
    Vector2 startPos = level.getPlayerStartPosition();
    std::cout << "Player start position: " << startPos.x << ", " << startPos.y << std::endl;
    player.Initialize(startPos);
    
    // Reset timers
    waveTimer = WAVE_DURATION;
    betweenWaveTimer = BETWEEN_WAVE_DURATION;
    playerInvincibilityTimer = 0;
    
    // Clear any existing enemies and powerups
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    
    for (Powerup* powerup : powerups) {
        delete powerup;
    }
    powerups.clear();
    
    playerBullets.clear();
    enemyBullets.clear();
    activePowerupTimers.clear();
    
    std::cout << "Game initialization complete" << std::endl;
}

void GameStateManager::Update(float deltaTime) {
    // Add emergency reset key
    if (IsKeyPressed(KEY_F2)) {
        std::cout << "Emergency reset triggered" << std::endl;
        Initialize();
        return;
    }
    
    // Test powerup spawn key
    if (IsKeyPressed(KEY_F3) && currentState == PLAYING) {
        SpawnTestPowerup();
    }
    
    // Convert milliseconds-based timing to seconds-based timing
    float dtMillis = deltaTime * 1000.0f;
    
    // Update active powerup timers
    for (auto it = activePowerupTimers.begin(); it != activePowerupTimers.end(); ) {
        it->second -= dtMillis;
        if (it->second <= 0) {
            it = activePowerupTimers.erase(it);
        } else {
            ++it;
        }
    }
    
    // Update player invincibility
    if (playerInvincibilityTimer > 0) {
        playerInvincibilityTimer -= dtMillis;
    }
    
    // Update based on current game state
    switch (currentState) {
        case START_MENU:
            // Menu handling happens elsewhere
            break;
            
        case PLAYING:
            UpdatePlaying(dtMillis);
            break;
            
        case DIED:
            UpdateDied(dtMillis);
            break;
            
        case BETWEEN_WAVES:
            UpdateBetweenWaves(dtMillis);
            break;
            
        case SHOPPING:
            UpdateShopping(dtMillis);
            break;
            
        case GAME_OVER:
            UpdateGameOver(dtMillis);
            break;
            
        case END_CUTSCENE:
            UpdateEndCutscene(dtMillis);
            break;
    }
}

void GameStateManager::UpdatePlaying(float dtMillis) {
    // Update wave timer
    if (waveTimer > 0) {
        waveTimer -= dtMillis;
    }
    
    // Convert milliseconds to seconds for updates
    float dtSeconds = dtMillis / 1000.0f;
    
    // Update player
    player.Update(dtSeconds);
    
    // Handle player shooting
    if (player.HasJustFired()) {
        Vector2 shootDir = player.GetCurrentShootDirection();
        Vector2 playerPos = player.GetPosition();
        int bulletDamage = player.GetBulletDamage();
        
        // Get bullet count based on player powerups
        int bulletCount = player.GetBulletCount();
        
        if (bulletCount == 1) {
            // Single bullet
            FirePlayerBullet(playerPos, shootDir, bulletDamage);
        } 
        else if (bulletCount == 3) {
            // Spread shot (3-way)
            FirePlayerBullet(playerPos, shootDir, bulletDamage); // Center
            
            // Calculate perpendicular directions for side bullets
            Vector2 perpDir = {-shootDir.y, shootDir.x};
            Vector2 dir1 = {
                shootDir.x * 0.866f + perpDir.x * 0.5f,
                shootDir.y * 0.866f + perpDir.y * 0.5f
            };
            Vector2 dir2 = {
                shootDir.x * 0.866f - perpDir.x * 0.5f,
                shootDir.y * 0.866f - perpDir.y * 0.5f
            };
            
            FirePlayerBullet(playerPos, dir1, bulletDamage);
            FirePlayerBullet(playerPos, dir2, bulletDamage);
        } 
        else if (bulletCount == 5) {
            // Shotgun (5-way)
            FirePlayerBullet(playerPos, shootDir, bulletDamage); // Center
            
            // Calculate angles for side bullets
            Vector2 perpDir = {-shootDir.y, shootDir.x};
            
            // 15 degree spread
            Vector2 dir1 = {
                shootDir.x * 0.966f + perpDir.x * 0.259f,
                shootDir.y * 0.966f + perpDir.y * 0.259f
            };
            Vector2 dir2 = {
                shootDir.x * 0.966f - perpDir.x * 0.259f,
                shootDir.y * 0.966f - perpDir.y * 0.259f
            };
            
            // 30 degree spread
            Vector2 dir3 = {
                shootDir.x * 0.866f + perpDir.x * 0.5f,
                shootDir.y * 0.866f + perpDir.y * 0.5f
            };
            Vector2 dir4 = {
                shootDir.x * 0.866f - perpDir.x * 0.5f,
                shootDir.y * 0.866f - perpDir.y * 0.5f
            };
            
            FirePlayerBullet(playerPos, dir1, bulletDamage);
            FirePlayerBullet(playerPos, dir2, bulletDamage);
            FirePlayerBullet(playerPos, dir3, bulletDamage);
            FirePlayerBullet(playerPos, dir4, bulletDamage);
        }
        
        // Reset the fire flag
        player.ResetJustFired();
    }
    
    // Update level
    level.update(dtSeconds);
    
    // Update bullets
    for (int i = static_cast<int>(playerBullets.size()) - 1; i >= 0; i--) {
        playerBullets[i].update(dtSeconds);
        if (!playerBullets[i].isActive()) {
            playerBullets.erase(playerBullets.begin() + i);
        }
    }
    
    for (int i = static_cast<int>(enemyBullets.size()) - 1; i >= 0; i--) {
        enemyBullets[i].update(dtSeconds);
        if (!enemyBullets[i].isActive()) {
            enemyBullets.erase(enemyBullets.begin() + i);
        }
    }
    
    // Update enemies
    Vector2 playerPos = player.GetPosition();
    for (int i = static_cast<int>(enemies.size()) - 1; i >= 0; i--) {
        bool shouldRemove = enemies[i]->update(playerPos, dtSeconds);
        if (shouldRemove || !enemies[i]->isActive()) {
            delete enemies[i];
            enemies.erase(enemies.begin() + i);
        }
    }
    
    // Check for enemy spawns from the queue
    int enemyType;
    Vector2 spawnPos;
    if (level.getNextSpawn(enemyType, spawnPos)) {
        SpawnEnemy(enemyType, spawnPos);
        std::cout << "Spawned enemy type " << enemyType << " at " << spawnPos.x << ", " << spawnPos.y << std::endl;
    }
    
    // Update powerups
    for (int i = powerups.size() - 1; i >= 0; i--) {
        powerups[i]->update(dtMillis);
        if (!powerups[i]->isActive()) {
            delete powerups[i];
            powerups.erase(powerups.begin() + i);
        }
    }
    
    // Check for collisions
    CheckCollisions();
    
    // Check for wave completion
    if (waveTimer <= 0 && enemies.empty() && level.isSpawnQueueEmpty()) {
        // Wave complete
        currentState = BETWEEN_WAVES;
        betweenWaveTimer = BETWEEN_WAVE_DURATION;
    }
}

void GameStateManager::UpdateDied(float dtMillis) {
    deathTimer -= dtMillis;
    
    if (deathTimer <= 0) {
        if (lives < 0) {
            // Game over
            SetGameState(GAME_OVER);
        } else {
            // Reset player position
            player.Initialize({384, 384});
            playerInvincibilityTimer = PLAYER_INVINCIBILITY_DURATION;
            SetGameState(PLAYING);
        }
    }
}

// Fix enemy health reference
void GameStateManager::UpdateEnemies(float dtMillis) {
    // Update existing enemies
    for (int i = enemies.size() - 1; i >= 0; i--) {
        Vector2 playerPos = player.GetPosition();
        if (enemies[i]->update(playerPos, dtMillis)) {
            // Enemy should be removed (e.g., special case)
            for (int h = 0; h < enemies[i]->getHealth(); h++) {
                score += 10;  // Award score for each health point
            }
            delete enemies[i];
            enemies.erase(enemies.begin() + i);
        }
    }
}

void GameStateManager::CheckCollisions() {
    // Player bullet collisions with enemies
    for (int b = playerBullets.size() - 1; b >= 0; b--) {
        Rectangle bulletRect = playerBullets[b].getBounds();
        
        for (int e = enemies.size() - 1; e >= 0; e--) {
            if (CheckCollisionRecs(bulletRect, enemies[e]->GetBounds())) {
                // Enemy hit by bullet
                int monsterHealth = enemies[e]->getHealth();
                
                if (enemies[e]->takeDamage(playerBullets[b].getDamage())) {
                    // Enemy defeated
                    int lootType = enemies[e]->getLootDrop();
                    
                    if (lootType != -1) {
                        // Spawn powerup at enemy position
                        Vector2 position = enemies[e]->GetPosition();
                        powerups.push_back(new Powerup(lootType, position, 7500));
                    }
                    
                    // Add score
                    score += 10;
                    
                    // Remove the enemy
                    delete enemies[e];
                    enemies.erase(enemies.begin() + e);
                }
                
                // Reduce bullet damage
                playerBullets[b].setDamage(playerBullets[b].getDamage() - monsterHealth);
                if (playerBullets[b].getDamage() <= 0) {
                    playerBullets.erase(playerBullets.begin() + b);
                    break;
                }
            }
        }
    }
    
    // Enemy bullet collisions with player
    if (playerInvincibilityTimer <= 0) {
        Rectangle playerRect = player.GetBounds();
        
        for (int i = enemyBullets.size() - 1; i >= 0; i--) {
            if (CheckCollisionRecs(playerRect, enemyBullets[i].getBounds())) {
                // Player hit
                PlayerDeath();
                enemyBullets.clear();
                break;
            }
        }
        
        // Enemy collision with player
        for (Enemy* enemy : enemies) {
            if (CheckCollisionRecs(playerRect, enemy->GetBounds())) {
                // Player hit
                PlayerDeath();
                break;
            }
        }
    }
    
    // Player collision with powerups
    Rectangle playerRect = player.GetBounds();
    for (int i = powerups.size() - 1; i >= 0; i--) {
        if (CheckCollisionRecs(playerRect, powerups[i]->getBounds())) {
            // Collect powerup
            int type = powerups[i]->getType();
            ActivatePowerup(type);
            
            // Remove the powerup
            delete powerups[i];
            powerups.erase(powerups.begin() + i);
        }
    }
}

void GameStateManager::PlayerDeath() {
    lives--;
    died = true;
    SetGameState(DIED);
    deathTimer = DEATH_DURATION;
    
    // Reset all active powerups
    activePowerupTimers.clear();
    
    // Clear all enemies and bullets if not in a shootout level
    if (whichWave % 4 != 0) {
        for (Enemy* enemy : enemies) {
            delete enemy;
        }
        enemies.clear();
        playerBullets.clear();
        enemyBullets.clear();
    }
    
    // Save game
    SaveGame();
}

void GameStateManager::SetGameState(GameState newState) {
    currentState = newState;
    
    // Handle state transition actions
    switch (newState) {
        case PLAYING:
            // Any setup needed for playing state
            break;
            
        case SHOPPING:
            SetupShop();
            break;
            
        case BETWEEN_WAVES:
            // Setup for between waves
            break;
            
        case DIED:
            // Setup for death sequence
            break;
            
        case GAME_OVER:
            // Game over setup
            break;
            
        default:
            break;
    }
}

void GameStateManager::ActivatePowerup(int type) {
    switch (type) {
        case COIN_1:  // En lugar de (int)COIN_1
            coins += 1;
            score += 10;
            break;
            
        case COIN_5:  // En lugar de (int)COIN_5
            coins += 5;
            score += 50;
            break;
            
        case POWERUP_LIFE:
            lives++;
            break;
            
        case POWERUP_SPEED:
            player.ActivatePowerup(POWERUP_SPEED, 10000.0f);
            activePowerupTimers[POWERUP_SPEED] = 10000.0f;
            break;
            
        case POWERUP_RAPIDFIRE:
            player.ActivatePowerup(POWERUP_RAPIDFIRE, 10000.0f);
            activePowerupTimers[POWERUP_RAPIDFIRE] = 10000.0f;
            break;
            
        case POWERUP_SPREAD:
            player.ActivatePowerup(POWERUP_SPREAD, 10000.0f);
            activePowerupTimers[POWERUP_SPREAD] = 10000.0f;
            break;
            
        case POWERUP_SHOTGUN:
            player.ActivatePowerup(POWERUP_SHOTGUN, 10000.0f);
            activePowerupTimers[POWERUP_SHOTGUN] = 10000.0f;
            break;
            
        case POWERUP_NUKE:
            // Implementa la lógica del powerup nuke
            break;
            
        default:
            break;
    }
}

bool GameStateManager::SaveGame() {
    // In a real implementation, save to a file
    // For now, just return success
    return true;
}

bool GameStateManager::LoadGame() {
    // In a real implementation, load from a file
    // For now, just return success
    return true;
}

void GameStateManager::Draw() {
    // Get offset for proper positioning
    Vector2 gameOffset = AssetsManager::GetGameOffset();
    float zoom = AssetsManager::GetZoom();
    
    // Clear background to BLACK
    ClearBackground(BLACK);
    
    // Get asset manager instance
    AssetsManager& assets = AssetsManager::getInstance();
    
    // Draw the current level
    level.draw(assets.spriteSheet, world);
    
    // Draw powerups with offset
    for (Powerup* powerup : powerups) {
        Vector2 pos = powerup->getPosition();
        // Store original position
        Vector2 origPos = pos;
        
        // Apply offset
        pos.x += gameOffset.x;
        pos.y += gameOffset.y;
        
        // Update position temporarily for drawing
        powerup->position = pos;
        powerup->draw();
        
        // Restore original position
        powerup->position = origPos;
    }
    
    // Draw bullets with offset
    for (size_t i = 0; i < playerBullets.size(); i++) {
        Vector2 pos = playerBullets[i].getPosition();
        // Store original position
        Vector2 origPos = pos;
        
        // Apply offset
        pos.x += gameOffset.x;
        pos.y += gameOffset.y;
        
        // Update position temporarily for drawing
        playerBullets[i].position = pos;
        playerBullets[i].draw(assets.spriteSheet);
        
        // Restore original position
        playerBullets[i].position = origPos;
    }
    
    for (size_t i = 0; i < enemyBullets.size(); i++) {
        Vector2 pos = enemyBullets[i].getPosition();
        // Store original position
        Vector2 origPos = pos;
        
        // Apply offset
        pos.x += gameOffset.x;
        pos.y += gameOffset.y;
        
        // Update position temporarily for drawing
        enemyBullets[i].position = pos;
        enemyBullets[i].draw(assets.spriteSheet);
        
        // Restore original position
        enemyBullets[i].position = origPos;
    }
    
    // Draw enemies with offset
    for (Enemy* enemy : enemies) {
        Vector2 pos = enemy->GetPosition();
        // Store original position
        Vector2 origPos = pos;
        
        // Apply offset
        pos.x += gameOffset.x;
        pos.y += gameOffset.y;
        
        // Update position temporarily for drawing
        enemy->position = pos;
        enemy->draw();
        
        // Restore original position
        enemy->position = origPos;
    }
    
    // Draw player with offset and handle invincibility effect
    bool shouldDrawPlayer = (playerInvincibilityTimer <= 0.0f || 
                           static_cast<int>(playerInvincibilityTimer / 100.0f) % 2 == 0);
    if (shouldDrawPlayer) {
        Vector2 pos = player.GetPosition();
        // Store original position
        Vector2 origPos = pos;
        
        // Apply offset
        pos.x += gameOffset.x;
        pos.y += gameOffset.y;
        
        // Update position temporarily for drawing
        player.position = pos;
        player.Draw(nullptr);
        
        // Restore original position
        player.position = origPos;
    }
    
    // Draw UI elements
    DrawUIElements();
    
    // Toggle debug info with F1 key
    if (IsKeyPressed(KEY_F1)) {
        showDebug = !showDebug;
        std::cout << "Debug display: " << (showDebug ? "ON" : "OFF") << std::endl;
    }
    
    // Draw debug info if enabled
    if (showDebug) {
        DrawDebugInfo();
    }
}

void GameStateManager::DrawDebugInfo() {
    // Display debug overlay
    DrawRectangle(10, 10, 300, 220, Fade(BLACK, 0.7f));
    DrawText("DEBUG INFO", 20, 20, 20, YELLOW);
    DrawText(TextFormat("Game State: %d", currentState), 20, 45, 16, WHITE);
    DrawText(TextFormat("Player Position: %.1f, %.1f", player.GetPosition().x, player.GetPosition().y), 20, 65, 16, WHITE);
    DrawText(TextFormat("Direction: %d", player.GetDirection()), 20, 85, 16, WHITE);
    DrawText(TextFormat("Wave: %d", whichWave), 20, 105, 16, WHITE);
    DrawText(TextFormat("Enemies: %d", enemies.size()), 20, 125, 16, WHITE);
    DrawText(TextFormat("FPS: %d", GetFPS()), 20, 145, 16, WHITE);
    DrawText(TextFormat("Spawn Queue Empty: %s", level.isSpawnQueueEmpty() ? "Yes" : "No"), 20, 165, 16, WHITE);
    DrawText(TextFormat("Level Size: %dx%d", level.getWidth(), level.getHeight()), 20, 185, 16, WHITE);
    DrawText(TextFormat("Keys: F1=Debug, F2=Reset", whichWave, lives), 20, 205, 16, WHITE);
    
    // Draw player movement keys state
    DrawRectangle(320, 10, 150, 145, Fade(BLACK, 0.7f));
    DrawText("Movement:", 330, 20, 16, WHITE);
    DrawText(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) ? "UP: ON" : "UP: off", 330, 40, 14, IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) ? GREEN : GRAY);
    DrawText(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S) ? "DOWN: ON" : "DOWN: off", 330, 60, 14, IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S) ? GREEN : GRAY);
    DrawText(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) ? "LEFT: ON" : "LEFT: off", 330, 80, 14, IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) ? GREEN : GRAY);
    DrawText(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) ? "RIGHT: ON" : "RIGHT: off", 330, 100, 14, IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) ? GREEN : GRAY);
    DrawText(IsKeyDown(KEY_SPACE) ? "SHOOT: ON" : "SHOOT: off", 330, 120, 14, IsKeyDown(KEY_SPACE) ? GREEN : GRAY);
    
    // Show a frame counter to detect lag or stuttering
    // Changed GetFrameCounter() to GetFramesCounter() to match Raylib API
    DrawText(TextFormat("Frame: %d", GetFrameTime()), 330, 140, 14, WHITE);
}

void GameStateManager::DrawUIElements() {
    // Draw lives counter
    // Draw coins counter
    // Draw score
    // Draw powerup icons
    
    // Wave progress bar
    if (currentState == PLAYING) {
        float waveProgress = waveTimer / WAVE_DURATION;
        // Draw wave progress bar
    }
    
    // Other UI elements based on game state
}

void GameStateManager::StartNextWave() {
    whichWave++;
    level.loadMap(whichWave);
    waveTimer = WAVE_DURATION;
    
    // Update monster spawn chances
    UpdateMonsterChancesForWave();
    
    // Clear existing enemies
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    
    // Set game state
    SetGameState(PLAYING);
}

void GameStateManager::SpawnEnemy(int type, Vector2 position) {
    Enemy* newEnemy = CreateEnemy((EnemyType)type, position);
    if (newEnemy) {
        enemies.push_back(newEnemy);
    }
}

void GameStateManager::FirePlayerBullet(Vector2 position, Vector2 direction, int damage) {
    Bullet newBullet(position, direction, damage, true);
    playerBullets.push_back(newBullet);
    // Play sound
    // ResourceManager::getInstance().getSound("player_shoot").play();
}

void GameStateManager::FireEnemyBullet(Vector2 position, Vector2 direction, int damage) {
    Bullet newBullet(position, direction, damage, false);
    enemyBullets.push_back(newBullet);
    // Play sound
    // ResourceManager::getInstance().getSound("enemy_shoot").play();
}

void GameStateManager::UpdateBetweenWaves(float deltaTime) {
    betweenWaveTimer -= deltaTime;
    
    if (betweenWaveTimer <= 0) {
        // Start the next wave
        if (whichWave % 4 == 0) {
            // Boss wave completed, show shopping
            SetGameState(SHOPPING);
        } else if (whichWave % 4 == 3) {
            // Pre-boss wave completed, show shopping
            SetGameState(SHOPPING);
        } else {
            // Regular wave, just start the next wave
            level.loadMap(whichWave);
            waveTimer = WAVE_DURATION;
            SetGameState(PLAYING);
        }
    }
}

void GameStateManager::UpdateShopping(float deltaTime) {
    // This would handle the shopping interface interactions
    // For now, pressing any key will exit the shop
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        level.loadMap(whichWave);
        waveTimer = WAVE_DURATION;
        SetGameState(PLAYING);
    }
}

void GameStateManager::UpdateGameOver(float deltaTime) {
    // This would handle game over screen interactions
    // For now, it just waits for R to be pressed for restart
    // which is handled in main.cpp
}

void GameStateManager::UpdateEndCutscene(float deltaTime) {
    // This would play the end cutscene
    // For now, pressing any key will return to menu
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        SetGameState(START_MENU);
    }
}

void GameStateManager::SpawnRandomEnemies(float deltaTime) {
    static float spawnTimer = 0.0f;
    
    spawnTimer -= deltaTime;
    if (spawnTimer <= 0) {
        // Reset timer
        spawnTimer = 2000.0f; // 2 seconds between random spawns
        
        // Check if we should spawn based on chance
        float rand = GetRandomValue(0, 1000) / 1000.0f;
        
        // Iterate through monster chances
        for (size_t i = 0; i < monsterChances.size(); i++) {
            if (rand < monsterChances[i].x) {
                // Spawn this monster type
                const std::vector<Vector2>& spawnPoints = level.getEnemySpawnPoints();
                if (!spawnPoints.empty()) {
                    // Get random spawn point
                    int spawnIndex = GetRandomValue(0, static_cast<int>(spawnPoints.size() - 1));
                    SpawnEnemy(static_cast<int>(i), spawnPoints[spawnIndex]);
                }
                break;
            }
        }
    }
}

void GameStateManager::SetupShop() {
    // Example of how to update references:
    // etc.
}

int GameStateManager::GetPriceForItem(int itemType) {
    // Return price based on item type
    switch (itemType) {
        case SHOP_ITEM_LIFE: return 10;
        case SHOP_ITEM_BOOT: return 5;
        case SHOP_ITEM_COFFEE: return 5;
        case SHOP_ITEM_HAT: return 7;
        case SHOP_ITEM_GLOVE: return 8;
        default: return 10; // Default price
    }
}

void GameStateManager::ApplyLevelSpecificSettings() {
    // Apply settings based on level
    // Different backgrounds, obstacles, etc.
}

void GameStateManager::UpdateMonsterChancesForWave() {
    // Update monster spawn chances based on current wave
    // Higher waves = harder monsters more likely
}

void GameStateManager::SpawnTestPowerup() {
    // Get a random position near the player
    Vector2 playerPos = player.GetPosition();
    Vector2 offset = {
        static_cast<float>(GetRandomValue(-100, 100)),
        static_cast<float>(GetRandomValue(-100, 100))
    };
    Vector2 position = {
        playerPos.x + offset.x,
        playerPos.y + offset.y
    };
    
    // Get a random powerup type (1-8, skipping COIN_1 at index 0)
    int type = GetRandomValue(1, 8);
    
    // Create the powerup
    powerups.push_back(new Powerup(type, position, 20000.0f)); // 20 seconds lifetime
}

void GameStateManager::HandleScreenSizeChange() {
    // Recalculate offsets based on new screen size
    Vector2 gameOffset = AssetsManager::GetGameOffset();
    
    std::cout << "Screen size changed. New offset: " << gameOffset.x << ", " << gameOffset.y << std::endl;
    
    // Update UI element positions if needed
    
    // Update level offset
    level.setOffset(gameOffset.x, gameOffset.y);
}