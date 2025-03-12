#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"

// Powerup types (matching the original game)
enum PowerupType {
    POWERUP_COIN_1,     // Single coin
    POWERUP_COIN_5,     // 5 coins
    POWERUP_LIFE,       // Extra life
    POWERUP_AMMO,       // Ammo for special weapons
    POWERUP_BOOT,       // Speed boost
    POWERUP_COFFEE,     // Shooting speed boost
    POWERUP_HAT,        // 3-way shooting
    POWERUP_GLOVE,      // Shotgun-style shooting
    POWERUP_NUKE,       // Clear all enemies
    POWERUP_COUNT
};

class Powerup {
public:
    // Constructor/Destructor
    Powerup(int type, Vector2 position, float lifetime = 10000.0f);
    ~Powerup();
    
    // Core functionality
    void update(float deltaTime);
    void draw();
    
    // Status
    bool isActive() const { return active; }
    
    // Properties
    int getType() const { return type; }
    Vector2 getPosition() const { return position; }
    Rectangle getBounds() const;
    
    // Add this to allow temporary position changes for drawing
    Vector2 position;  // Make it public or add a setter

private:
    // Core properties
    int type;
    bool active;
    
    // Visual properties
    float animationTimer;
    int currentFrame;
    
    // Lifetime
    float remainingLifetime;
    float blinkTimer;
    bool visible;  // For blinking when about to expire
};

#endif // POWERUP_H