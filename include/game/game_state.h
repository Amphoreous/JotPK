#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include <vector>
#include <map>
#include "game/player.h"
#include "game/enemy.h"
#include "game/bullet.h"
#include "game/powerup.h"
#include "game/level.h"

// Game states
enum GameState {
    START_MENU,
    PLAYING,
    DIED,
    BETWEEN_WAVES,
    SHOPPING,
    GAME_OVER,
    END_CUTSCENE
};

// Powerup types for shop - renamed to avoid conflicts with PowerupType
enum ShopItemType {
    SHOP_ITEM_LIFE,
    SHOP_ITEM_BOOT,
    SHOP_ITEM_COFFEE,
    SHOP_ITEM_HAT,
    SHOP_ITEM_GLOVE
};

class GameStateManager {
public:
    GameStateManager();
    ~GameStateManager();
    
    void Initialize();
    void Update(float deltaTime);
    void Draw();
    
    bool SaveGame();
    bool LoadGame();
    
    // Getters for game state
    GameState GetCurrentState() const { return currentState; }
    int GetLives() const { return lives; }
    int GetCoins() const { return coins; }
    int GetScore() const { return score; }
    
private:
    // Debug helpers
    void DrawDebugInfo();
    void SpawnTestPowerup();
    bool showDebug = false;
    
    // Game state
    GameState currentState;
    int whichWave;
    int world;
    int whichRound;
    int lives;
    int coins;
    int score;
    bool died;
    
    // Game elements
    Player player;
    Level level;
    std::vector<Enemy*> enemies;
    std::vector<Powerup*> powerups;
    std::vector<Bullet> playerBullets;
    std::vector<Bullet> enemyBullets;
    
    // Timers
    float waveTimer;
    float betweenWaveTimer;
    float deathTimer;
    float shopTimer;
    float playerInvincibilityTimer;
    
    // Items
    int heldItemType;
    std::map<int, float> activePowerupTimers;
    
    // Monster spawn chances [type index] = {chance, amountMultiplier}
    std::vector<Vector2> monsterChances;
    
    // State update methods
    void UpdatePlaying(float deltaTime);
    void UpdateDied(float deltaTime);
    void UpdateBetweenWaves(float deltaTime);
    void UpdateShopping(float deltaTime);
    void UpdateGameOver(float deltaTime);
    void UpdateEndCutscene(float deltaTime);
    
    // Game mechanics
    void SetGameState(GameState newState);
    void UpdateEnemies(float deltaTime);
    void SpawnRandomEnemies(float deltaTime);
    void CheckCollisions();
    void PlayerDeath();
    void ActivatePowerup(int type);
    void StartNextWave();
    void SpawnEnemy(int type, Vector2 position);
    void FirePlayerBullet(Vector2 position, Vector2 direction, int damage);
    void FireEnemyBullet(Vector2 position, Vector2 direction, int damage);
    void SetupShop();
    int GetPriceForItem(int itemType);
    void ApplyLevelSpecificSettings();
    void UpdateMonsterChancesForWave();
    void DrawUIElements();
};

#endif