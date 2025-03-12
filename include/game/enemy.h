#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <vector>
#include <string>

// Enemy types from original game
enum EnemyType {
    ENEMY_ORC,
    ENEMY_GHOST,
    ENEMY_OGRE,
    ENEMY_MUMMY,
    ENEMY_DEVIL,
    ENEMY_MUSHROOM,
    ENEMY_SPIKEY,
    ENEMY_BOSS_COWBOY,
    ENEMY_BOSS_FECTOR,
    ENEMY_TYPE_COUNT
};

// Base enemy class
class Enemy {
public:
    // Constructor/Destructor
    Enemy(EnemyType type, Vector2 position, int health);
    virtual ~Enemy();
    
    // Core functionality
    virtual bool update(Vector2 playerPos, float deltaTime);
    virtual void draw();
    
    // Status
    bool isActive() const { return active; }
    bool takeDamage(int damage);
    int getHealth() const { return health; }
    
    // Properties
    Vector2 GetPosition() const { return position; }
    Rectangle GetBounds() const;
    EnemyType GetType() const { return type; }
    
    // Enemy behavior
    virtual bool shouldShoot(Vector2 playerPos) { return false; }
    virtual Vector2 getShootDirection(Vector2 playerPos);
    virtual int getShootDamage() const { return 1; }
    
    // Loot drops
    virtual int getLootDrop();
    
    // Add this to allow temporary position changes for drawing
    Vector2 position;  // Make it public or add a setter

protected:
    // Core properties
    EnemyType type;
    int health;
    bool active;
    
    // Animation
    float animationTimer;
    int currentFrame;
    int frameCount;
    float animationSpeed;
    
    // Movement
    Vector2 moveDirection;
    float speed;
    float movementTimer;     // For enemies that change direction
    float attackCooldown;    // For shooting enemies
    
    // AI state
    bool aggressive;         // If true, enemy chases player
    float playerDetectRange; // How close player must be to trigger chase
    
    // Attributes that vary by enemy type
    int damage;              // Damage done on collision
    int scoreValue;          // Points for defeating this enemy
    int coinDropChance;      // Percentage chance to drop a coin
    int powerupDropChance;   // Percentage chance to drop a powerup
    
    // Helper methods
    void updateAnimation(float deltaTime);
    Vector2 getRandomDirection();
    virtual void calculateMovement(Vector2 playerPos, float deltaTime);
};

// Factory function to create enemies
Enemy* CreateEnemy(EnemyType type, Vector2 position);

#endif // ENEMY_H