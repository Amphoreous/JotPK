#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "game/game_defs.h"
#include "game/level.h"
#include <set>
#include <map>
#include <vector>

class Level;

class Player {
private:
    Vector2 position;
    float animationTimer;
    int currentFrame;
    float movementSpeed;
    Direction direction;
    int runSpeedLevel;
    float zombieModeTimer;
    std::map<int, float> activePowerups;
    bool justFired;
    Vector2 shootDirection;
    int bulletDamage;
    int bulletCount;
    bool shouldDraw;
    std::set<Direction> shootingDirections;
    float shotTimer;
    float shootingDelay;
    int fireSpeedLevel;

public:
    Player();
    void Initialize(Vector2 startPosition);
    void Update(float deltaTime, Level* level);
    void Draw(Texture2D* optionalTexture = nullptr);
    
    // Getters
    Vector2 GetPosition() const { return position; }
    Direction GetDirection() const { return direction; }
    bool HasJustFired() const { return justFired; }
    Vector2 GetCurrentShootDirection() const { return shootDirection; }
    int GetBulletDamage() const { return bulletDamage; }
    int GetBulletCount() const { return bulletCount; }
    Rectangle GetBounds() const;
    
    // Setters
    void SetRunSpeedLevel(int level);
    void SetZombieMode(float duration);
    void ResetJustFired() { justFired = false; }
    void SetPosition(Vector2 pos) { position = pos; }
    void SetPosition(float x, float y) { position = {x, y}; }
    
    // Powerup methods
    bool HasPowerup(int type) const;
    void AddPowerup(int type, float duration);
    void ActivatePowerup(int type);

private:
    void HandleInput(float deltaTime, Level* level);
    void Move(Vector2 moveDir, float deltaTime, Level* level);
    void UpdatePowerups(float deltaTime);
    void UpdateDirection(Vector2 moveDir);
    bool IsCollidingWithMap(Rectangle playerBox, Level* level);
    void SpawnBullet(Direction dir);
    void SpawnBulletsInAllDirections();
    void addPlayerShootingDirection(Direction dir);
};

#endif