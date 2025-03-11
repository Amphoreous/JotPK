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

// Game states matching the original game
enum GameState {
    START_MENU,
    PLAYING,
    DIED,
    BETWEEN_WAVES,
    SHOPPING,
    GAME_OVER,
    END_CUTSCENE
};

class GameStateManager {
public:
    GameStateManager();
    ~GameStateManager();
    
    void Initialize();
    void Update(float deltaTime);
    void Draw();
    
    // Game state management
    GameState GetCurrentState() const { return currentState; }
    void SetGameState(GameState newState);
    
    // Wave and level management
    int GetCurrentWave() const { return whichWave; }
    int GetCurrentWorld() const { return world; }
    void StartNextWave();
    
    // Player stats
    int GetLives() const { return lives; }
    int GetCoins() const { return coins; }
    int GetScore() const { return score; }
    void AddScore(int points) { score += points; }
    void AddCoins(int amount) { coins += amount; }
    
    // Powerup system
    void ActivatePowerup(int type);
    void AddHeldItem(int type) { heldItemType = type; }
    
    // Enemy management
    void SpawnEnemy(int type, Vector2 position);
    void UpdateEnemies(float deltaTime);
    
    // Bullet management
    void FirePlayerBullet(Vector2 position, Vector2 direction, int damage);
    void FireEnemyBullet(Vector2 position, Vector2 direction, int damage);
    
    // Save/load functionality
    bool SaveGame();
    bool LoadGame();
    
private:
    // Core game state
    GameState currentState;
    int whichWave;
    int world;
    int whichRound;  // New game+ counter
    int lives;
    int coins;
    int score;
    bool died;
    
    // Timers
    float waveTimer;
    float betweenWaveTimer;
    float deathTimer;
    float shopTimer;
    float playerInvincibilityTimer;
    
    // Game elements
    Player player;
    std::vector<Enemy*> enemies;
    std::vector<Bullet> playerBullets;
    std::vector<Bullet> enemyBullets;
    std::vector<Powerup*> powerups;
    Level level;
    
    // Powerup management
    int heldItemType;
    std::map<int, float> activePowerupTimers;
    
    // Monster spawning chances
    std::vector<Vector2> monsterChances;  // x = chance, y = additional spawn chance
    
    // Helper methods
    void UpdatePlaying(float deltaTime);
    void UpdateDied(float deltaTime);
    void UpdateBetweenWaves(float deltaTime);
    void UpdateShopping(float deltaTime);
    void UpdateGameOver(float deltaTime);
    void UpdateEndCutscene(float deltaTime);
    
    void CheckCollisions();
    void SpawnRandomEnemies(float deltaTime);
    void PlayerDeath();
    
    // Shopping/merchant system
    void SetupShop();
    int GetPriceForItem(int itemType);
    
    // Game progression
    void ApplyLevelSpecificSettings();
    void UpdateMonsterChancesForWave();
    
    // UI Drawing
    void DrawUIElements();
};

#endif // GAME_STATE_H