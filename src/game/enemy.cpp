#include "game/enemy.h"
#include "game/game_defs.h"  // Include the new header with definitions
#include "game/raylib_wrapper.h"
#include "game/assets_manager.h"
#include <cmath>
#include <stdlib.h>

// Constants for enemy behavior
#define ANIMATION_SPEED_DEFAULT 0.15f
#define DIRECTION_CHANGE_TIME 2.0f
#define ATTACK_COOLDOWN_DEFAULT 1.5f

// Enemy constructor
Enemy::Enemy(EnemyType type, Vector2 position, int health) :
    type(type),
    position(position),
    health(health),
    active(true),
    animationTimer(0),
    currentFrame(0),
    frameCount(4),  // Default frame count
    animationSpeed(ANIMATION_SPEED_DEFAULT),
    moveDirection({0, 0}),
    speed(0),  // Will be set based on type
    movementTimer(0),
    attackCooldown(0),
    aggressive(false),
    playerDetectRange(0),
    damage(1),
    scoreValue(10),
    coinDropChance(10),
    powerupDropChance(5)
{
    // Initialize with random direction
    moveDirection = getRandomDirection();
    
    // Set type-specific properties
    switch (type) {
    case ENEMY_ORC:
        speed = 60.0f;
        frameCount = 4;
        aggressive = false;
        playerDetectRange = 0;  // Non-aggressive
        damage = 1;
        scoreValue = 10;
        coinDropChance = 15;
        powerupDropChance = 2;
        break;
        
    case ENEMY_GHOST:
        speed = 70.0f;
        frameCount = 4;
        aggressive = true;
        playerDetectRange = 250.0f;
        damage = 1;
        scoreValue = 15;
        coinDropChance = 20;
        powerupDropChance = 3;
        break;
        
    case ENEMY_OGRE:
        speed = 50.0f;
        frameCount = 4;
        aggressive = true;
        playerDetectRange = 200.0f;
        health = 3;  // More health than basic enemies
        damage = 1;
        scoreValue = 25;
        coinDropChance = 25;
        powerupDropChance = 5;
        break;
        
    case ENEMY_MUMMY:
        speed = 40.0f;
        frameCount = 4;
        aggressive = true;
        playerDetectRange = 300.0f;
        health = 4;
        damage = 1;
        scoreValue = 30;
        coinDropChance = 30;
        powerupDropChance = 7;
        break;
        
    case ENEMY_DEVIL:
        speed = 90.0f;
        frameCount = 4;
        aggressive = true;
        playerDetectRange = 400.0f;
        health = 2;
        damage = 1;
        scoreValue = 50;
        coinDropChance = 40;
        powerupDropChance = 10;
        break;
        
    case ENEMY_MUSHROOM:
        speed = 80.0f;
        frameCount = 4;
        aggressive = false;
        playerDetectRange = 0;  // Non-aggressive
        health = 1;
        damage = 1;
        scoreValue = 20;
        coinDropChance = 35;
        powerupDropChance = 4;
        break;
        
    case ENEMY_SPIKEY:
        speed = 110.0f;
        frameCount = 4;
        aggressive = true;
        playerDetectRange = 350.0f;
        health = 1;
        damage = 1;
        scoreValue = 40;
        coinDropChance = 45;
        powerupDropChance = 8;
        break;
        
    case ENEMY_BOSS_COWBOY:
        speed = 80.0f;
        frameCount = 4;
        aggressive = true;
        playerDetectRange = 500.0f;
        health = 20;
        damage = 2;
        scoreValue = 500;
        coinDropChance = 100;  // Always drops coins
        powerupDropChance = 100;  // Always drops powerup
        break;
        
    case ENEMY_BOSS_FECTOR:
        speed = 100.0f;
        frameCount = 4;
        aggressive = true;
        playerDetectRange = 600.0f;
        health = 30;
        damage = 2;
        scoreValue = 1000;
        coinDropChance = 100;  // Always drops coins
        powerupDropChance = 100;  // Always drops powerup
        break;
    }
}

Enemy::~Enemy() {
    // No resources to clean up
}

bool Enemy::update(Vector2 playerPos, float deltaTime) {
    if (!active) return false;
    
    // Update timers
    animationTimer += deltaTime;
    movementTimer += deltaTime;
    
    if (attackCooldown > 0) {
        attackCooldown -= deltaTime;
    }
    
    // Update animation
    updateAnimation(deltaTime);
    
    // Calculate movement based on AI behavior
    calculateMovement(playerPos, deltaTime);
    
    // Apply movement
    position.x += moveDirection.x * speed * deltaTime;
    position.y += moveDirection.y * speed * deltaTime;
    
    // Keep enemy within screen bounds (could be modified for specific enemies)
    position.x = Clamp(position.x, 16, 768 - 16);
    position.y = Clamp(position.y, 16, 768 - 16);
    
    return false; // Return true if enemy should be removed
}

void Enemy::updateAnimation(float deltaTime) {
    if (animationTimer >= animationSpeed) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

Vector2 Enemy::getRandomDirection() {
    // Generate random angle in radians
    float angle = (float)GetRandomValue(0, 359) * PI / 180.0f;
    
    // Convert to direction vector
    return Vector2{cosf(angle), sinf(angle)};
}

void Enemy::calculateMovement(Vector2 playerPos, float deltaTime) {
    float distToPlayer = sqrtf((position.x - playerPos.x) * (position.x - playerPos.x) + 
                              (position.y - playerPos.y) * (position.y - playerPos.y));
    
    // Update movement timer
    movementTimer += deltaTime;
    
    // Change direction periodically
    if (movementTimer >= DIRECTION_CHANGE_TIME) {
        // Reset timer
        movementTimer = 0;
        
        // If not aggressive or player is out of detection range, move randomly
        if (!aggressive || distToPlayer > playerDetectRange) {
            // Use individual component assignment instead of direct assignment
            Vector2 randomDir = getRandomDirection();
            moveDirection.x = randomDir.x;
            moveDirection.y = randomDir.y;
        }
    }
    
    // Aggressive enemies chase the player when within range
    if (aggressive) {
        if (distToPlayer <= playerDetectRange) {
            // Calculate direction to player
            float dx = playerPos.x - position.x;
            float dy = playerPos.y - position.y;
            
            // Normalize the vector
            float length = sqrtf(dx * dx + dy * dy);
            if (length > 0) {
                moveDirection.x = dx / length;
                moveDirection.y = dy / length;
            }
        }
    }
    
    // Rest of function...
}

void Enemy::draw() {
    if (!active) return;
    
    AssetsManager& assets = AssetsManager::getInstance();
    
    // Get sprite based on enemy type and animation frame
    Rectangle src = assets.getEnemySprite((int)type, currentFrame);
    
    // Draw the enemy
    DrawTexturePro(
        assets.spriteSheet,
        src,
        Rectangle{position.x - 8, position.y - 8, 16, 16},
        Vector2{8, 8},
        0.0f,
        WHITE
    );
    
    // Draw health indicators for multi-hit enemies
    if (health > 1) {
        for (int i = 0; i < health; i++) {
            DrawRectangle(
                (int)position.x - 8 + i * 4,
                (int)position.y - 12,
                3,
                2,
                RED
            );
        }
    }
}

bool Enemy::takeDamage(int damage) {
    health -= damage;
    
    // Return true if the enemy died
    if (health <= 0) {
        active = false;
        return true;
    }
    
    return false;
}

Vector2 Enemy::getShootDirection(Vector2 playerPos) {
    // Calculate direction to player
    Vector2 dirToPlayer = {
        playerPos.x - position.x,
        playerPos.y - position.y
    };
    
    // Normalize
    float length = sqrtf(dirToPlayer.x * dirToPlayer.x + dirToPlayer.y * dirToPlayer.y);
    if (length > 0) {
        dirToPlayer.x /= length;
        dirToPlayer.y /= length;
    }
    
    return dirToPlayer;
}

Rectangle Enemy::GetBounds() const {
    float size = (type == ENEMY_BOSS_COWBOY || type == ENEMY_BOSS_FECTOR) ? 48.0f : 32.0f;
    
    return Rectangle{
        position.x - size/2 + 4,  // Slightly smaller hitbox than visual
        position.y - size/2 + 4,  // for better gameplay feel
        size - 8,
        size - 8
    };
}

int Enemy::getLootDrop() {
    // Check coin drop
    if (GetRandomValue(0, 99) < coinDropChance) {
        return 0;  // Coin powerup
    }
    
    // Check powerup drop
    if (GetRandomValue(0, 99) < powerupDropChance) {
        // Return random powerup type (1-7)
        return GetRandomValue(1, 7);
    }
    
    return -1;  // No drop
}

// Specialized enemy types

// Shooting enemy class
class ShootingEnemy : public Enemy {
public:
    ShootingEnemy(EnemyType type, Vector2 position, int health) 
        : Enemy(type, position, health) {
        // Set shooting-specific properties
        attackCooldown = ATTACK_COOLDOWN_DEFAULT;
    }
    
    bool update(Vector2 playerPos, float deltaTime) override {
        // Call base update
        bool result = Enemy::update(playerPos, deltaTime);
        
        // Update attack cooldown
        if (attackCooldown <= 0) {
            // Ready to shoot
            if (shouldShoot(playerPos)) {
                attackCooldown = ATTACK_COOLDOWN_DEFAULT;
                // The actual shooting happens in the game state manager
            }
        }
        
        return result;
    }
    
    bool shouldShoot(Vector2 playerPos) override {
        if (attackCooldown > 0) return false;
        
        // Check if player is in shooting range
        float distToPlayer = Vector2Distance(position, playerPos);
        return distToPlayer <= playerDetectRange * 1.2f;  // Shoot from slightly further than chase range
    }
};

// Factory function to create enemies
Enemy* CreateEnemy(EnemyType type, Vector2 position) {
    // Determine initial health based on enemy type
    int initialHealth;
    
    switch (type) {
        case ENEMY_ORC:
        case ENEMY_GHOST:
        case ENEMY_MUSHROOM:
        case ENEMY_SPIKEY:
            initialHealth = 1;
            break;
            
        case ENEMY_DEVIL:
            initialHealth = 2;
            break;
            
        case ENEMY_OGRE:
            initialHealth = 3;
            break;
            
        case ENEMY_MUMMY:
            initialHealth = 4;
            break;
            
        case ENEMY_BOSS_COWBOY:
            initialHealth = 20;
            break;
            
        case ENEMY_BOSS_FECTOR:
            initialHealth = 30;
            break;
            
        default:
            initialHealth = 1;
            break;
    }
    
    // Create basic or shooting enemy based on type
    if (type == ENEMY_GHOST || type == ENEMY_DEVIL || 
        type == ENEMY_BOSS_COWBOY || type == ENEMY_BOSS_FECTOR) {
        return new ShootingEnemy(type, position, initialHealth);
    } else {
        return new Enemy(type, position, initialHealth);
    }
}