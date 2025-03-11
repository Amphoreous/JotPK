#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <vector>

// Player movement directions (8-way)
enum PlayerDirection {
    UP,
    UP_RIGHT,
    RIGHT,
    DOWN_RIGHT,
    DOWN,
    DOWN_LEFT,
    LEFT,
    UP_LEFT,
    IDLE
};

class Player {
public:
    Player();
    ~Player();
    
    // Core functionality
    void Initialize(Vector2 startPosition);
    void Update(float deltaTime);
    void Draw(Texture2D* playerTextures);
    
    // Movement
    void Move(Vector2 direction, float deltaTime);
    PlayerDirection GetDirection() const { return direction; }
    Vector2 GetPosition() const { return position; }
    
    // Collision
    Rectangle GetBounds() const;
    
    // Status
    bool IsInvincible() const { return invincibilityTimer > 0; }
    void SetInvincible(float duration) { invincibilityTimer = duration; }
    
    // Shooting
    Vector2 GetShootingDirection() const;
    bool CanShoot() const { return shootTimer <= 0; }
    void ResetShootTimer() { shootTimer = GetShootDelay(); }
    
    // Powerups
    bool HasPowerup(int type) const;
    float GetPowerupTimeRemaining(int type) const;
    void ActivatePowerup(int type, float duration);
    void DeactivatePowerup(int type);
    
    // Player stats
    float GetMovementSpeed() const;
    float GetShootDelay() const;
    int GetBulletCount() const;  // For spread/shotgun
    int GetBulletDamage() const;
    
private:
    // Core properties
    Vector2 position;
    PlayerDirection direction;
    float animationTimer;
    int currentFrame;
    
    // Movement
    float baseMovementSpeed;
    float movementSpeed;
    bool lastMoveWasKeyboard;
    
    // Shooting
    float shootTimer;
    float baseShootDelay;
    
    // Status
    float invincibilityTimer;
    
    // Powerups
    std::vector<std::pair<int, float>> activePowerups;  // type, time remaining
    
    // Animation helper methods
    void UpdateAnimation(float deltaTime);
    int GetAnimationFrame() const;
    
    // Input handling
    void HandleKeyboardInput(float deltaTime);
    void HandleGamepadInput(float deltaTime);
};

#endif // PLAYER_H