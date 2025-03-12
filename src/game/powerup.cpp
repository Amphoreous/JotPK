#include "game/powerup.h"
#include "game/assets_manager.h"
#include <cmath> // For sinf

// Constants
#define POWERUP_ANIMATION_SPEED 0.1f
#define POWERUP_FRAME_COUNT 4
#define POWERUP_BLINK_TIME 3000.0f  // Start blinking 3 seconds before expiring
#define POWERUP_BLINK_RATE 150.0f   // Blink every 150ms

Powerup::Powerup(int type, Vector2 position, float lifetime) :
    type(type),
    position(position),
    active(true),
    animationTimer(0),
    currentFrame(0),
    remainingLifetime(lifetime),
    blinkTimer(0),
    visible(true)
{
}

Powerup::~Powerup() {
    // No resources to clean up
}

void Powerup::update(float deltaTime) {
    // Update animation
    animationTimer += deltaTime;
    if (animationTimer >= POWERUP_ANIMATION_SPEED) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % POWERUP_FRAME_COUNT;
    }
    
    // Update lifetime
    if (remainingLifetime > 0) {
        remainingLifetime -= deltaTime;
        
        // Handle blinking when about to expire
        if (remainingLifetime < POWERUP_BLINK_TIME) {
            blinkTimer += deltaTime;
            if (blinkTimer >= POWERUP_BLINK_RATE) {
                blinkTimer = 0;
                visible = !visible;
            }
        }
        
        // Deactivate if expired
        if (remainingLifetime <= 0) {
            active = false;
        }
    }
}

void Powerup::draw() {
    if (!active || !visible) return;
    
    AssetsManager& assets = AssetsManager::getInstance();
    
    // Get sprite based on powerup type
    Rectangle src = assets.getPowerupSprite(type);
    
    // Add a bob effect for powerups
    float yOffset = sinf(GetTime() * 4) * 2.0f;
    
    // Draw the powerup
    DrawTexturePro(
        assets.spriteSheet,
        src,
        Rectangle{position.x - 8, position.y - 8 + yOffset, 16, 16},
        Vector2{8, 8},
        0.0f,
        WHITE
    );
}

Rectangle Powerup::getBounds() const {
    return Rectangle{
        position.x - 8,
        position.y - 8,
        16,
        16
    };
}