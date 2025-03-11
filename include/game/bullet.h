#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"

// Direction enumeration for bullet sprites
enum BulletDirection {
    BULLET_UP,
    BULLET_UP_RIGHT,
    BULLET_RIGHT,
    BULLET_DOWN_RIGHT,
    BULLET_DOWN,
    BULLET_DOWN_LEFT,
    BULLET_LEFT,
    BULLET_UP_LEFT
};

class Bullet {
public:
    // Constructors/Destructors
    Bullet();
    Bullet(Vector2 position, Vector2 direction, int damage, bool isPlayerBullet);
    ~Bullet();
    
    // Core functionality
    void update(float deltaTime = 1.0f/60.0f);
    void draw(Texture2D texture);
    
    // Status checks
    bool isActive() const { return active; }
    void setActive(bool value) { active = value; }
    
    // Properties
    Vector2 getPosition() const { return position; }
    Vector2 getDirection() const { return direction; }
    BulletDirection getVisualDirection() const { return visualDirection; }
    int getDamage() const { return damage; }
    void setDamage(int value) { damage = value; }
    bool isFromPlayer() const { return playerBullet; }
    
    // Collision
    Rectangle getBounds() const;
    
private:
    // Core properties
    Vector2 position;
    Vector2 direction;  // Normalized direction vector
    BulletDirection visualDirection;  // Direction for sprite selection
    bool active;
    int damage;
    bool playerBullet;  // True if from player, false if from enemy
    
    // Movement
    float speed;
    float lifetime;  // How long the bullet has existed
    float maxLifetime;  // Maximum lifetime before despawning
    
    // Determines the visual direction based on a direction vector
    BulletDirection calculateVisualDirection(Vector2 dir);
};

#endif // BULLET_H