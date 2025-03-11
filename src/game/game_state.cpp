#include "game/game_state.h"
#include "game/game_defs.h"  // Include the new header with definitions
#include "game/assets_manager.h"
#include "resource_manager.h"  // Include resource manager
#include <algorithm>
#include <cstdlib>

#define WAVE_DURATION 80000.0f
#define BETWEEN_WAVE_DURATION 5000.0f
#define DEATH_DURATION 3000.0f
#define PLAYER_INVINCIBILITY_DURATION 5000.0f

GameStateManager::GameStateManager() {
    // Initialize default values
    currentState = START_MENU;
    whichWave = 0;
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
    // Setup initial game state
    currentState = START_MENU;
    
    // Initialize player
    player.Initialize({384, 384});
    
    // Initialize level
    level.loadMap(whichWave);
    
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
}

void GameStateManager::Update(float deltaTime) {
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
    
    // Update player
    player.Update(dtMillis);
    
    // Update bullets
    for (int i = static_cast<int>(playerBullets.size()) - 1; i >= 0; i--) {
        playerBullets[i].update();
        if (!playerBullets[i].isActive()) {
            playerBullets.erase(playerBullets.begin() + i);
        }
    }
    
    for (int i = static_cast<int>(enemyBullets.size()) - 1; i >= 0; i--) {
        enemyBullets[i].update();
        if (!enemyBullets[i].isActive()) {
            enemyBullets.erase(enemyBullets.begin() + i);
        }
    }
    
    // Update enemies
    UpdateEnemies(dtMillis);
    
    // Spawn random enemies based on current wave settings
    SpawnRandomEnemies(dtMillis);
    
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
        // Wave completed
        whichWave++;
        SetGameState(BETWEEN_WAVES);
        betweenWaveTimer = BETWEEN_WAVE_DURATION;
        
        // Update monster chances for the next wave
        UpdateMonsterChancesForWave();
        
        // Save game progress
        SaveGame();
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
        case (int)COIN_1:
            // Add coins
            coins += 1;
            break;
            
        case (int)COIN_5:
            // Add coins
            coins += 5;
            break;
            
        case POWERUP_LIFE:
            lives++;
            // Play powerup sound
            break;
            
        case POWERUP_SPEED:
        case POWERUP_SPREAD:
        case POWERUP_RAPIDFIRE:
        case POWERUP_SHOTGUN:
            // Activate timed powerup
            activePowerupTimers[type] = 10000.0f;  // 10 seconds
            // Play powerup sound
            break;
            
        case POWERUP_NUKE:
            // Kill all enemies instantly
            for (Enemy* enemy : enemies) {
                delete enemy;
            }
            enemies.clear();
            // Play explosion sound
            break;
            
        // Add other powerup types as needed
            
        default:
            // Store as held item
            heldItemType = type;
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
    // Get asset manager instance
    AssetsManager& assets = AssetsManager::getInstance();
    
    // Draw the current level
    level.draw(assets.spriteSheet, world, 0.0f);
    
    // Draw powerups
    for (Powerup* powerup : powerups) {
        powerup->draw();
    }
    
    // Draw bullets
    for (Bullet& bullet : playerBullets) {
        bullet.draw(assets.spriteSheet);
    }
    
    for (Bullet& bullet : enemyBullets) {
        bullet.draw(assets.spriteSheet);
    }
    
    // Draw enemies
    for (Enemy* enemy : enemies) {
        enemy->draw();
    }
    
    // Draw player (with invincibility effect if applicable)
    bool shouldDrawPlayer = (playerInvincibilityTimer <= 0.0f || 
                           static_cast<int>(playerInvincibilityTimer / 100.0f) % 2 == 0);
    if (shouldDrawPlayer) {
        player.Draw(nullptr);  // We're not using the passed texture array anymore
    }
    
    // Draw UI elements
    DrawUIElements();
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
                if (!level.getEnemySpawnPoints().empty()) {
                    // Get random spawn point
                    int spawnIndex = GetRandomValue(0, static_cast<int>(level.getEnemySpawnPoints().size() - 1));
                    SpawnEnemy(static_cast<int>(i), level.getEnemySpawnPoints()[spawnIndex]);
                }
                break;
            }
        }
    }
}

void GameStateManager::SetupShop() {
    // This would setup the shop interface and items
}

int GameStateManager::GetPriceForItem(int itemType) {
    // Return price based on item type
    switch (itemType) {
        case POWERUP_LIFE: return 10;
        case POWERUP_BOOT: return 5;
        case POWERUP_COFFEE: return 5;
        case POWERUP_HAT: return 7;
        case POWERUP_GLOVE: return 10;
        default: return 5;
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