#pragma once
#include "raylib.h"
#include "gameplay/PrairieKing.hpp"
#include <vector>

class CowboyMonster : public PrairieKing {
public:
    static const int MONSTER_ANIMATION_DELAY = 500;
    static const int BASE_TILE_SIZE = 16;
    static const int PIXEL_ZOOM = 3;
    static const int ORC = 0;
    static const int GHOST = 1;
    static const int OGRE = 2;
    static const int MUMMY = 3;
    static const int DEVIL = 4;
    static const int MUSHROOM = 5;
    static const int SPIKEY = 6;
    static const int ORC_SPEED = 2;
    static const int OGRE_SPEED = 1;
    static const int GHOST_SPEED = 3;
    static const int SPIKEY_SPEED = 3;
    static const int ORC_HEALTH = 1;
    static const int GHOST_HEALTH = 1;
    static const int OGRE_HEALTH = 3;
    static const int SPIKEY_HEALTH = 2;

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

    CowboyMonster(int which, int health, int speed, Vector2 position);
    CowboyMonster(int which, Vector2 position);
    virtual ~CowboyMonster() = default;

    virtual void Draw(const Texture2D& texture, Vector2 topLeftScreenCoordinate);
    virtual bool TakeDamage(int damage);
    virtual int GetLootDrop();
    virtual bool Move(Vector2 playerPosition, float deltaTime);
    void SpikeyEndBehavior(int extraInfo);
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

    Dracula();
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

    Outlaw(Vector2 position, int health);
    void Draw(const Texture2D& texture, Vector2 topLeftScreenCoordinate) override;
    bool Move(Vector2 playerPosition, float deltaTime) override;
    int GetLootDrop() override;
    bool TakeDamage(int damage) override;
};
