#pragma once
#include "AssetManager.hpp"
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Forward declarations
class Dracula;
class Outlaw;

// Game constants namespace to avoid duplication
namespace GameConstants {
    constexpr int MONSTER_ANIMATION_DELAY = 500;
    constexpr int BASE_TILE_SIZE = 16;
    constexpr int PIXEL_ZOOM = 3;
    constexpr int ORC = 0;
    constexpr int GHOST = 1;
    constexpr int OGRE = 2;
    constexpr int MUMMY = 3;
    constexpr int DEVIL = 4;
    constexpr int MUSHROOM = 5;
    constexpr int SPIKEY = 6;
    constexpr int ORC_SPEED = 2;
    constexpr int OGRE_SPEED = 1;
    constexpr int GHOST_SPEED = 3;
    constexpr int SPIKEY_SPEED = 3;
    constexpr int ORC_HEALTH = 0;
    constexpr int GHOST_HEALTH = 1;
    constexpr int OGRE_HEALTH = 3;
    constexpr int SPIKEY_HEALTH = 2;
    constexpr int WAVE_DURATION = 80000;        // 80 seconds per wave
    constexpr int BETWEEN_WAVE_DURATION = 5000; // 5 seconds between waves
    constexpr int START_MENU_DURATION = 1500;   // 1.5 seconds for start menu
    constexpr int DEATH_DELAY = 3000;           // 3 seconds death delay
    constexpr int PLAYER_INVINCIBLE_DURATION = 5000; // 5 seconds invincibility after death
}

// Equality operators for raylib types
inline bool operator==(const Rectangle& lhs, const Rectangle& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y &&
        lhs.width == rhs.width && lhs.height == rhs.height;
}

// Custom hash functions for raylib types
namespace std {
    template<>
    struct hash<Vector2> {
        size_t operator()(const Vector2& vec) const {
            // Combine the hash of both Vector2 components
            size_t seed = 0;
            seed ^= std::hash<float>()(vec.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<float>()(vec.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };

    template<>
    struct hash<Rectangle> {
        size_t operator()(const Rectangle& rect) const {
            // Use hash_combine technique for combining hashes
            size_t seed = 0;
            // Combine the hash of all Rectangle components using boost's hash_combine technique
            seed ^= std::hash<float>()(rect.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<float>()(rect.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<float>()(rect.width) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<float>()(rect.height) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}

class PrairieKing {
public:
    // Forward declarations
    class CowboyBullet;
    class CowboyPowerup;
    class JOTPKProgress;
    class CowboyMonster;

    // Enum declarations
    enum class GameKeys {
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
        ShootLeft,
        ShootRight,
        ShootUp,
        ShootDown,
        UsePowerup,
        SelectOption,
        Exit,
        MAX
    };

    // Helper functions
    std::vector<Vector2> GetBorderPoints();
    static float GetRandomFloat(float min, float max);
    static int GetRandomInt(int min, int max);
    Vector2 GetRandomVector2(float minX, float maxX, float minY, float maxY);

    // Delegate type for motion pause behavior
    using BehaviorAfterMotionPause = std::function<void(int)>;

    // Constants (ported directly from AbigailGame)
    static constexpr int MAP_WIDTH = 16;
    static constexpr int MAP_HEIGHT = 16;
    static constexpr int PIXEL_ZOOM = 3;
    static constexpr int BULLET_SPEED = 23;
    static constexpr double LOOT_CHANCE = 0.05;
    static constexpr double COIN_CHANCE = 0.05;
    static constexpr int LOOT_DURATION = 7500;
    static constexpr int POWERUP_DURATION = 10000;
    static constexpr float PLAYER_SPEED = 2.0f;
    static constexpr int BASE_TILE_SIZE = 16;
    static constexpr int ORC_SPEED = 2;
    static constexpr int OGRE_SPEED = 1;
    static constexpr int GHOST_SPEED = 3;
    static constexpr int SPIKEY_SPEED = 3;
    static constexpr int ORC_HEALTH = 1;
    static constexpr int GHOST_HEALTH = 1;
    static constexpr int OGRE_HEALTH = 3;
    static constexpr int SPIKEY_HEALTH = 2;
    static constexpr int CACTUS_DANCE_DELAY = 800;
    static constexpr int PLAYER_MOTION_DELAY = 100;
    static constexpr int PLAYER_FOOTSTEP_DELAY = 200;
    static constexpr int DEATH_DELAY = 3000;

    // Map tile types
    static constexpr int MAP_BARRIER1 = 0;
    static constexpr int MAP_BARRIER2 = 1;
    static constexpr int MAP_ROCKY1 = 2;
    static constexpr int MAP_DESERT = 3;
    static constexpr int MAP_GRASSY = 4;
    static constexpr int MAP_CACTUS = 5;
    static constexpr int MAP_FENCE = 7;
    static constexpr int MAP_TRENCH1 = 8;
    static constexpr int MAP_TRENCH2 = 9;
    static constexpr int MAP_BRIDGE = 10;

    // Monster types
    static constexpr int ORC = 0;
    static constexpr int GHOST = 1;
    static constexpr int OGRE = 2;
    static constexpr int MUMMY = 3;
    static constexpr int DEVIL = 4;
    static constexpr int MUSHROOM = 5;
    static constexpr int SPIKEY = 6;
    static constexpr int DRACULA = 7;

    // World types
    static constexpr int DESERT_WORLD = 0;
    static constexpr int GRAVEYARD = 1;
    static constexpr int WOODS = 2;

    // Powerup types
    static constexpr int POWERUP_LOG = -1;
    static constexpr int POWERUP_SKULL = -2;
    static constexpr int POWERUP_HEART = -3;
    static constexpr int COIN1 = 0;
    static constexpr int COIN5 = 1;
    static constexpr int POWERUP_SPREAD = 2;
    static constexpr int POWERUP_RAPIDFIRE = 3;
    static constexpr int POWERUP_NUKE = 4;
    static constexpr int POWERUP_ZOMBIE = 5;
    static constexpr int POWERUP_SPEED = 6;
    static constexpr int POWERUP_SHOTGUN = 7;
    static constexpr int POWERUP_LIFE = 8;
    static constexpr int POWERUP_TELEPORT = 9;
    static constexpr int POWERUP_SHERRIFF = 10;

    // Store item types
    static constexpr int ITEM_FIRESPEED1 = 0;
    static constexpr int ITEM_FIRESPEED2 = 1;
    static constexpr int ITEM_FIRESPEED3 = 2;
    static constexpr int ITEM_RUNSPEED1 = 3;
    static constexpr int ITEM_RUNSPEED2 = 4;
    static constexpr int ITEM_LIFE = 5;
    static constexpr int ITEM_AMMO1 = 6;
    static constexpr int ITEM_AMMO2 = 7;
    static constexpr int ITEM_AMMO3 = 8;
    static constexpr int ITEM_SPREADPISTOL = 9;
    static constexpr int ITEM_STAR = 10;
    static constexpr int ITEM_SKULL = 11;
    static constexpr int ITEM_LOG = 12;

    // Game over options
    static constexpr int OPTION_RETRY = 0;
    static constexpr int OPTION_QUIT = 1;

    // Wave constants
    static constexpr int WAVE_DURATION = 80000;
    static constexpr int BETWEEN_WAVE_DURATION = 5000;

    // Inner class definitions
    class CowboyPowerup {
    public:
        int which;
        Vector2 position;
        int duration;
        float yOffset;

        CowboyPowerup(int which, Vector2 position, int duration);
        void Draw(const Texture2D& cursorTexture, Vector2 topLeftScreenCoordinate);
    };

    class JOTPKProgress {
    public:
        int bulletDamage;
        int fireSpeedLevel;
        int ammoLevel;
        bool spreadPistol;
        int runSpeedLevel;
        int lives;
        int coins;
        int score;
        bool died;
        int whichRound;
        int whichWave;
        int heldItem;
        int world;
        int waveTimer;
        std::vector<Vector2> monsterChances;

        JOTPKProgress();
    };

    class CowboyBullet {
    public:
        Vector2 position;
        Vector2 motion;
        int damage;

        CowboyBullet(Vector2 position, Vector2 motion, int damage);
        CowboyBullet(Vector2 position, int direction, int damage);
    };

    class TemporaryAnimatedSprite {
    public:
        Rectangle sourceRect;
        Vector2 position;
        float animationInterval;
        int frames;
        int currentFrame;
        float timer;
        float rotation;
        float scale;
        bool flipped;
        float layerDepth;
        Color tint;
        int delayBeforeAnimationStart;
        std::function<void(int)> endFunction;
        int extraData;
        float alpha; // Added alpha property for transparency effects

        TemporaryAnimatedSprite(Rectangle sourceRect, float interval, int frameCount,
            int startFrame, Vector2 pos, float rot, float scale,
            bool flip, float depth, Color color);
        bool Update(float deltaTime);
        void Draw(const Texture2D& texture);
    };

    class CowboyMonster {
    public:
        int health;
        int type;
        int speed;
        float movementAnimationTimer;
        Rectangle position;
        int movementDirection;
        bool movedLastTurn;
        bool oppositeMotionGuy;
        bool invisible;
        bool special;
        bool uninterested;
        bool flyer;
        Color tint;
        Color flashColor;
        float flashColorTimer;
        int ticksSinceLastMovement;
        Vector2 acceleration;
        Vector2 targetPosition;

        CowboyMonster(AssetManager& assets, int which, int health, int speed, Vector2 position);
        CowboyMonster(AssetManager& assets, int which, Vector2 position);
        virtual ~CowboyMonster() = default;

        virtual void Draw(const Texture2D& texture, Vector2 topLeftScreenCoordinate);
        virtual bool TakeDamage(int damage);
        virtual int GetLootDrop();
        virtual bool Move(Vector2 playerPosition, float deltaTime);
        void SpikeyEndBehavior(int extraInfo);

        // Funciones auxiliares para el movimiento
        Vector2 GetVelocityTowardPoint(Vector2 start, Vector2 end, float speed);
        int GetRandomInt(int min, int max);
        float GetRandomFloat(float min, float max);
    };

    class Dracula : public CowboyMonster {
    public:
        static const int GLOATING_PHASE = -1;
        static const int WALK_RANDOMLY_AND_SHOOT_PHASE = 0;
        static const int SPREAD_SHOT_PHASE = 1;
        static const int SUMMON_DEMON_PHASE = 2;
        static const int SUMMON_MUMMY_PHASE = 3;

        int phase;
        int phaseInternalTimer;
        int phaseInternalCounter;
        int shootTimer;
        int fullHealth;
        Vector2 homePosition;

        Dracula(AssetManager& assets);
        void Draw(const Texture2D& texture, Vector2 topLeftScreenCoordinate) override;
        int GetLootDrop() override;
        bool TakeDamage(int damage) override;
        bool Move(Vector2 playerPosition, float deltaTime) override;
        void FireSpread(Vector2 origin, double offsetAngle);
        void SummonEnemies(Vector2 origin, int which);
    };

    class Outlaw : public CowboyMonster {
    public:
        static const int TALKING_PHASE = -1;
        static const int HIDING_PHASE = 0;
        static const int DART_OUT_AND_SHOOT_PHASE = 1;
        static const int RUN_AND_GUN_PHASE = 2;
        static const int RUN_GUN_AND_PANT_PHASE = 3;
        static const int SHOOT_AT_PLAYER_PHASE = 4;

        int phase;
        int phaseCountdown;
        int shootTimer;
        int phaseInternalTimer;
        int phaseInternalCounter;
        bool dartLeft;
        int fullHealth;
        Vector2 homePosition;

        Outlaw(AssetManager& assets, Vector2 position, int health);
        void Draw(const Texture2D& texture, Vector2 topLeftScreenCoordinate) override;
        bool Move(Vector2 playerPosition, float deltaTime) override;
        int GetLootDrop() override;
        bool TakeDamage(int damage) override;
    };

    PrairieKing(AssetManager& assets);
    ~PrairieKing() = default;

    void Initialize();
    void Update(float deltaTime);
    void Draw();
    bool IsGameOver() const { return m_gameOver; }

    // Game state functions
    bool LoadGame();
    void SaveGame();
    void Reset();
    void ApplyNewGamePlus();
    void ApplyLevelSpecificStates();

    // Gameplay functions
    float GetMovementSpeed(float speed, int directions);
    bool GetPowerUp(CowboyPowerup c);
    void UsePowerup(int which);
    static void AddGuts(Vector2 position, int whichGuts);
    void EndOfGopherAnimationBehavior2(int extraInfo);
    void EndOfGopherAnimationBehavior(int extraInfo);
    static void KillOutlaw();
    void UpdateBullets(float deltaTime);
    void PlayerDie();
    void AfterPlayerDeathFunction(int extra);
    void StartNewRound();
    void ProcessInputs();
    void SpawnBullets(const std::vector<int>& directions, Vector2 spawn);
    bool IsSpawnQueueEmpty();
    static bool IsMapTilePassable(int tileType);
    static bool IsMapTilePassableForMonsters(int tileType);
    bool IsCollidingWithMonster(Rectangle r, CowboyMonster* subject);
    bool IsCollidingWithMapForMonsters(Rectangle positionToCheck);
    bool IsCollidingWithMap(Rectangle positionToCheck);
    bool IsCollidingWithMap(Vector2 position);
    void AddPlayerMovementDirection(int direction);
    void AddPlayerShootingDirection(int direction);
    void StartShoppingLevel();
    int GetPriceForItem(int whichItem);
    void GetMap(int wave, int(&newMap)[MAP_WIDTH][MAP_HEIGHT]);
    std::vector<Vector2> GetBorderPoints(const Rectangle& rect);

    // Helper functions for rendering and resource access
    Texture2D GetTexture(const std::string& name);
    Sound GetSound(const std::string& name);
    static Sound GetSoundStatic(const std::string& name);
    Rectangle GetRectForShopItem(int itemID);
    JOTPKProgress GetProgress() const;
    void SetButtonState(GameKeys key, bool pressed);
    void UpdatePlayer(float deltaTime);
    void StartNewWave();
    void AddMonster(CowboyMonster* monster);
    void AddTemporarySprite(const TemporaryAnimatedSprite& sprite);

    // Static function to get the current game instance
    static PrairieKing* GetGameInstance();

    // Helper functions for input
    bool IsKeyPressed(GameKeys key);
    bool IsKeyDown(GameKeys key);
    bool IsKeyReleased(GameKeys key);

    // Helper functions for monster spawning
    std::vector<Vector2> GetMonsterChancesForWave(int wave);
    Vector2 GetRandomSpawnPosition();
    int ChooseMonsterType(const std::vector<Vector2>& chances);

    // Wave and timer related functions
    void UpdateWaveTimer(float deltaTime);
    void UpdateBetweenWaveTimer(float deltaTime);
    void HandleWaveCompletion();
    void SpawnMonstersForWave();
    void UpdateMonsterChancesForWave();

private:
    // Asset references
    AssetManager& m_assets;

    // Game state
    bool m_isGameOver;
    bool m_gameOver;
    bool m_quit;
    bool m_died;
    bool m_onStartMenu;
    bool m_shopping;
    bool m_gopherRunning;
    bool m_store;
    bool m_merchantLeaving;
    bool m_merchantArriving;
    bool m_merchantShopOpen;
    bool m_waitingForPlayerToMoveDownAMap;
    bool m_scrollingMap;
    bool m_hasGopherAppeared;
    bool m_shootoutLevel;
    bool m_gopherTrain;
    bool m_playerJumped;
    bool m_endCutscene;
    bool m_spreadPistol;

    Sound m_overworldSong;

    // Game variables
    int m_runSpeedLevel;
    int m_fireSpeedLevel;
    int m_ammoLevel;
    int m_whichRound;
    int m_bulletDamage;
    int m_speedBonus;
    int m_fireRateBonus;
    int m_lives;
    int m_coins;
    int m_score;
    int m_shootingDelay;
    int m_shotTimer;
    int m_motionPause;
    int m_gameOverOption;
    int m_gameRestartTimer;
    int m_fadeThenQuitTimer;
    int m_whichWave;
    int m_monsterConfusionTimer;
    int m_zombieModeTimer;
    int m_shoppingTimer;
    int m_holdItemTimer;
    int m_itemToHold;
    int m_newMapPosition;
    int m_playerInvincibleTimer;
    int m_screenFlash;
    int m_gopherTrainPosition;
    int m_endCutsceneTimer;
    int m_endCutscenePhase;
    int m_startTimer;
    float m_deathTimer;
    int m_waveTimer;
    int m_betweenWaveTimer;
    static int m_world;
    float m_monsterClearTimer = -1.0f;
    bool m_waveCompleted = false;

    // Game objects
    Vector2 m_playerPosition;
    Rectangle m_playerBoundingBox;
    Rectangle m_merchantBox;
    Rectangle m_noPickUpBox;
    Rectangle m_gopherBox;
    Vector2 m_gopherMotion;
    Rectangle m_shoppingCarpetNoPickup;
    Vector2 m_topLeftScreenCoordinate;
    float m_cactusDanceTimer;
    float m_playerAnimationTimer = 0.0f;
    float m_playerFootstepSoundTimer = 200.0f;
    float m_playerMotionAnimationTimer = 0.0f;
    BehaviorAfterMotionPause m_behaviorAfterPause;

    // Collections
    std::vector<CowboyMonster*> m_monsters;
    std::unordered_set<Vector2> m_borderTiles;
    std::vector<int> m_playerMovementDirections;
    std::vector<int> m_playerShootingDirections;
    std::vector<CowboyBullet> m_bullets;
    std::vector<CowboyBullet> m_enemyBullets;
    std::vector<CowboyPowerup> m_powerups;
    std::vector<TemporaryAnimatedSprite> m_temporarySprites;
    std::unique_ptr<CowboyPowerup> m_heldItem; // Changed from raw pointer to unique_ptr
    std::unordered_map<Rectangle, int, std::hash<Rectangle>> m_storeItems;

    // Data structures needed for game state
    std::vector<std::vector<std::pair<int, int>>> m_spawnQueue;
    std::vector<Vector2> m_monsterChances;
    int m_map[MAP_WIDTH][MAP_HEIGHT];
    int m_nextMap[MAP_WIDTH][MAP_HEIGHT]; // Add buffer for next map
    std::unordered_map<int, int> m_activePowerups;

    // Input handling
    std::unordered_set<GameKeys> m_buttonHeldState;
    std::unordered_map<GameKeys, int> m_buttonHeldFrames;

    int GetTileSize() const { return BASE_TILE_SIZE * PIXEL_ZOOM; }
};