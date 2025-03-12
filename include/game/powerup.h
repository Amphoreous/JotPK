#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"
#include "game/game_defs.h" // Añade esta línea para usar PowerupType desde aquí

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