#pragma once
#include "raylib.h"
#include "../AssetManager.hpp"
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <functional>

class PrairieKing {
public:
    // Forward declarations of nested classes
    class CowboyBullet;
    class CowboyMonster;
    class Dracula;
    class CowboyPowerup;

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

    enum class MapTile {
        Barrier1 = 0,
        Barrier2 = 1,
        Rocky1 = 2,
        Desert = 3,
        Grassy = 4,
        Cactus = 5,
        Fence = 7,
        Trench1 = 8,
        Trench2 = 9,
        Bridge = 10
    };

    enum class MonsterType {
        Orc = 0,
        Ghost = 1,
        Ogre = 2,
        Mummy = 3,
        Devil = 4,
        Mushroom = 5,
        Spikey = 6,
        Dracula = 7
    };

    enum class World {
        Desert = 0,
        Graveyard = 1,
        Woods = 2
    };

    enum class PowerupType {
        Log = -1,
        Skull = -2,
        Heart = -3,
        Coin1 = 0,
        Coin5 = 1,
        Spread = 2,
        RapidFire = 3,
        Nuke = 4,
        Zombie = 5,
        Speed = 6,
        Shotgun = 7,
        Life = 8,
        Teleport = 9,
        Sheriff = 10
    };

    // Game constants from AbigailGame.cs
    static constexpr int MAP_WIDTH = 16;
    static constexpr int MAP_HEIGHT = 16;
    static constexpr int PIXEL_ZOOM = 3;
    static constexpr int BULLET_SPEED = 8;
    static constexpr double LOOT_CHANCE = 0.05;
    static constexpr double COIN_CHANCE = 0.05;
    static constexpr int BASE_TILE_SIZE = 16;
    static constexpr int TILE_SIZE = BASE_TILE_SIZE * PIXEL_ZOOM;

    // Monster constants
    static constexpr int ORC_SPEED = 2;
    static constexpr int OGRE_SPEED = 1;
    static constexpr int GHOST_SPEED = 3;
    static constexpr int SPIKEY_SPEED = 3;
    static constexpr int ORC_HEALTH = 1;
    static constexpr int GHOST_HEALTH = 1;
    static constexpr int OGRE_HEALTH = 3;
    static constexpr int SPIKEY_HEALTH = 2;

    // Animation and timing constants
    static constexpr int CACTUS_DANCE_DELAY = 800;
    static constexpr int PLAYER_MOTION_DELAY = 100;
    static constexpr int PLAYER_FOOTSTEP_DELAY = 200;
    static constexpr int DEATH_DELAY = 3000;
    static constexpr int WAVE_DURATION = 80000;
    static constexpr int BETWEEN_WAVE_DURATION = 5000;

    PrairieKing(AssetManager& assets);
    ~PrairieKing();

    void Initialize();
    void Update(float deltaTime);
    void Draw();
    bool IsGameOver() const;

    // Helper methods for monsters
    void SpawnEnemyBullet(const Vector2& position, const Vector2& motion);
    void SpawnMonsterAtPosition(MonsterType type, const Vector2& position);

    // Friend declarations to allow access from monster classes
    friend class Dracula;
    friend class CowboyMonster;

    const Vector2& GetScreenOffset() const { return m_topLeftScreenCoordinate; }

private:
    AssetManager& m_assets;
    Vector2 m_topLeftScreenCoordinate;
    std::set<GameKeys> m_buttonHeldState;
    std::map<GameKeys, int> m_buttonHeldFrames;
    
    // Game state
    bool m_died;
    bool m_onStartMenu;
    bool m_gameOver;
    bool m_shopping;
    bool m_merchantShopOpen;
    bool m_waitingForPlayerToMoveDownAMap;
    bool m_scrollingMap;
    bool m_hasGopherAppeared;
    bool m_shootoutLevel;
    bool m_gopherTrain;
    bool m_playerJumped;
    bool m_endCutscene;
    bool m_isTransitioningToGameOver;
    float m_gameOverTransitionTimer;
    float m_fadeOutAlpha;
    
    // Game stats
    int m_lives;
    int m_coins;
    int m_score;
    int m_bulletDamage;
    int m_whichWave;
    int m_waveTimer;
    int m_betweenWaveTimer;
    World m_currentWorld;

    // Player state
    Vector2 m_playerPosition;
    Rectangle m_playerBoundingBox;
    float m_playerMotionAnimationTimer;
    float m_playerFootstepSoundTimer;
    int m_playerInvincibleTimer;
    int m_deathTimer;

    // Map state
    std::vector<std::vector<int>> m_map;
    std::vector<std::vector<int>> m_nextMap;
    int m_newMapPosition;

    // Powerup management
    static constexpr int POWERUP_DURATION = 10000;
    static constexpr int LOOT_DURATION = 7500;
    
    std::map<PowerupType, int> m_activePowerups;
    std::vector<std::unique_ptr<CowboyPowerup>> m_powerups;
    std::unique_ptr<CowboyPowerup> m_heldItem;
    
    // Wave management
    bool InitializeWave(int wave);
    void UpdateWave(float deltaTime);
    void StartNewWave();
    bool IsCollidingWithMap(const Rectangle& rect) const;
    void AddBarrier(int x, int y, int type);
    void AddCactus(int x, int y);
    void SpawnMonster(MonsterType type);
    void UpdateMonsterChances();

    std::vector<std::unique_ptr<CowboyMonster>> m_monsters;
    int m_gameOverOption;
    bool m_shouldQuit;

    int m_runSpeedLevel;
    int m_fireSpeedLevel;
    int m_ammoLevel;
    bool m_spreadPistol;
    std::vector<Vector2> m_monsterChances;

    // Player movement and shooting
    void UpdatePlayerMovement(float deltaTime);
    void UpdatePlayerShooting(float deltaTime);
    void SpawnBullet(const Vector2& position, int direction, float angleOffset = 0.0f);

    // Bullet management
    std::vector<std::unique_ptr<CowboyBullet>> m_bullets;
    std::vector<std::unique_ptr<CowboyBullet>> m_enemyBullets;
    int m_shotTimer;
    int m_shootingDelay;

    // Helper methods
    void InitializeMap();
    void UpdateInput();
    void UpdateStartMenu(float deltaTime);
    void UpdateGameOver(float deltaTime);
    void UpdatePlayerBoundingBox();
    void StartShoppingLevel();
    void UsePowerup(int powerupType);
    
    // Gameplay mechanics
    void UpdateBullets(float deltaTime);
    void UpdatePowerups(float deltaTime);
    void UpdateMonsters(float deltaTime);
    void UpdateMonsterChances();
    void UpdateWave(float deltaTime);
    void PlayerDie();
    void CollectPowerup(CowboyPowerup* powerup);
    void SpawnLoot(const Vector2& position, int lootType);

    // Game state
    int m_zombieModeTimer;  // Change from bool to int
    bool m_gopherRunning;    // Add missing state variable

    // New helper methods
    void TransitionToGameOver();
    void UpdateGameOverTransition(float deltaTime);
    void DrawTransitionEffects();

    // Add missing state variables
    bool m_beatLevelWithAbigail;
    bool m_playingWithAbigail;
    bool m_store;
    bool m_merchantLeaving;
    bool m_merchantArriving;
    Rectangle m_shoppingCarpetNoPickup;
    Rectangle m_merchantBox;
    int m_gamerestartTimer;
    int m_fadethenQuitTimer;
    int m_abigailPortraitTimer;
    int m_abigailPortraitExpression;
    int m_monsterConfusionTimer;
    int m_holdItemTimer;
    int m_screenFlash;
    std::string m_abigailDialogue;

    // Add missing player 2 state
    Vector2 m_player2Position;
    Rectangle m_player2BoundingBox;
    int m_player2deathTimer;
    int m_player2invincibleTimer;
    float m_player2FootstepSoundTimer;
    int m_player2AnimationTimer;
    int m_player2TargetUpdateTimer;
    int m_player2shotTimer;
    CowboyMonster* m_targetMonster;

    // Add missing sound/music state
    Music m_overworldSong;
    Music m_outlawSong;
    Music m_zombieSong;

    // Add missing methods from AbigailGame
    void LoadGame();
    void SaveGame();
    void ApplyNewGamePlus();
    void StartAbigailPortrait(int expression, std::string dialogue);
    void AfterPlayerDeathFunction(int extra);
    void EndOfGopherAnimationBehavior(int extraInfo);
    void EndOfGopherAnimationBehavior2(int extraInfo);
    static void KillOutlaw();
    static void AddGuts(Point position, int whichGuts);

    // Nested class definitions
    class CowboyBullet {
    public:
        CowboyBullet(const Vector2& position, const Vector2& motion, int damage);
        CowboyBullet(const Vector2& position, int direction, int damage) {
            this->position = position;
            this->damage = damage;
            
            // Set motion based on direction
            switch (direction) {
                case 0: // Up
                    motion = {0, -BULLET_SPEED};
                    break;
                case 1: // Right
                    motion = {BULLET_SPEED, 0};
                    break;
                case 2: // Down
                    motion = {0, BULLET_SPEED};
                    break;
                case 3: // Left
                    motion = {-BULLET_SPEED, 0};
                    break;
            }
        }
        void Update();
        void Draw(const Vector2& screenOffset, Texture2D texture);
        Rectangle GetBounds() const { return {position.x, position.y, 12, 12}; }
        Vector2 GetPosition() const { return position; }
        Vector2 GetMotion() const { return motion; }
        int GetDamage() const { return damage; }
    private:
        Vector2 position;
        Vector2 motion;
        int damage;
    };

    class CowboyPowerup {
    public:
        CowboyPowerup(int type, const Vector2& position, int duration) 
            : m_type(type), m_position(position), m_duration(duration), 
              m_yOffset(0), m_blinkTimer(0) {}
              
        void Update(float deltaTime) {
            m_duration -= static_cast<int>(deltaTime * 1000.0f);
            m_blinkTimer += deltaTime;
            m_yOffset = sinf(m_blinkTimer * 5.0f) * 5.0f;
        }
        
        void Draw(const Vector2& screenOffset, Texture2D texture) {
            if (m_duration > 2000 || m_duration / 200 % 2 == 0) {
                Vector2 drawPos = {
                    screenOffset.x + m_position.x,
                    screenOffset.y + m_position.y + m_yOffset
                };
                Rectangle sourceRect = {272.0f + m_type * 16, 1808, 16, 16};
                DrawTextureRec(texture, sourceRect, drawPos, WHITE);
            }
        }
        int GetType() const { return m_type; }
        Rectangle GetBounds() const { return {m_position.x, m_position.y, 48, 48}; }
        Vector2 GetPosition() const { return m_position; }
        int GetDuration() const { return m_duration; }
    private:
        int m_type;
        Vector2 m_position;
        int m_duration;
        float m_yOffset;
        float m_blinkTimer;
    };

    class CowboyMonster {
    public:
        static constexpr int MONSTER_ANIMATION_DELAY = 500;
        CowboyMonster(MonsterType type, const Vector2& position);
        virtual ~CowboyMonster() = default;  // Add virtual destructor
        virtual void Update(float deltaTime, const Vector2& playerPos);
        virtual void Draw(const Vector2& screenOffset, Texture2D spritesheet);
        virtual bool TakeDamage(int damage);
        virtual int GetLootDrop() const;
        Rectangle GetBounds() const { return bounds; }
        bool IsCollidingWith(const Rectangle& other) const;
        Vector2 GetPosition() const;
    protected:
        friend class PrairieKing;  // Add this line to allow access to protected members
        friend class Dracula;  // Add Dracula as friend if needed
        void InitializeMonster();
        MonsterType type;
        Rectangle bounds;
        int health;
        int speed;
        float movementAnimationTimer;
        Vector2 acceleration;
        Vector2 targetPosition;
        bool uninterested;
        bool flyer;
        bool invisible;
        bool special;
        Color tint;
        float flashColorTimer;
        int ticksSinceLastMovement;
        int movementDirection;
        bool movedLastTurn;
        bool oppositeMotionGuy;
    };

    class Dracula : public CowboyMonster {
    public:
        static constexpr int GLOATING_PHASE = -1;
        static constexpr int WALK_RANDOMLY_AND_SHOOT_PHASE = 0;
        static constexpr int SPREAD_SHOT_PHASE = 1;
        static constexpr int SUMMON_DEMON_PHASE = 2;
        static constexpr int SUMMON_MUMMY_PHASE = 3;

        Dracula(PrairieKing* game);
        void Update(float deltaTime, const Vector2& playerPos) override;
        void Draw(const Vector2& screenOffset, Texture2D spritesheet) override;
        bool TakeDamage(int damage) override;
        int GetLootDrop() const override;
    private:
        PrairieKing* m_game;  // Game instance pointer
        int m_phase;
        int m_phaseInternalTimer;
        int m_phaseInternalCounter;
        int m_shootTimer;
        int m_fullHealth;
        Vector2 m_homePosition;
        void UpdateWalkAndShootPhase(float deltaTime, const Vector2& playerPos);
        void UpdateSpreadShotPhase(float deltaTime, const Vector2& playerPos);
        void UpdateSummonPhase(float deltaTime, const Vector2& playerPos);
        void FireSpreadShot();
        void SummonMinions(MonsterType type);
    };

    class Outlaw : public CowboyMonster {
    public:
        static constexpr int TALKING_PHASE = -1;
        static constexpr int HIDING_PHASE = 0;
        static constexpr int DART_OUT_AND_SHOOT_PHASE = 1;
        static constexpr int RUN_AND_GUN_PHASE = 2;
        static constexpr int RUN_GUN_AND_PANT_PHASE = 3;
        static constexpr int SHOOT_AT_PLAYER_PHASE = 4;

        Outlaw(PrairieKing* game);
        void Update(float deltaTime, const Vector2& playerPos) override;
        void Draw(const Vector2& screenOffset, Texture2D spritesheet) override;
        bool TakeDamage(int damage) override;
        int GetLootDrop() const override;

    private:
        PrairieKing* m_game;
        int m_phase;
        int m_phaseCountdown;
        int m_shootTimer;
        int m_phaseInternalTimer;
        int m_phaseInternalCounter;
        bool m_dartLeft;
        int m_fullHealth;
        Vector2 m_homePosition;

        void UpdateHidingPhase(float deltaTime);
        void UpdateDartAndShootPhase(float deltaTime, const Vector2& playerPos);
        void UpdateRunAndGunPhase(float deltaTime, const Vector2& playerPos);
        void UpdateShootAtPlayerPhase(float deltaTime, const Vector2& playerPos);
    };
};
