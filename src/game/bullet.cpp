#include "game/bullet.h"
#include <cmath>
#include "game/assets_manager.h"

// Constants
#define PLAYER_BULLET_SPEED 400.0f
#define ENEMY_BULLET_SPEED 200.0f
#define BULLET_MAX_LIFETIME 2.0f  // 2 seconds max lifetime

Bullet::Bullet() : 
    position({0, 0}),
    direction({0, 0}),
    visualDirection(BULLET_UP),
    active(false),
    damage(1),
    playerBullet(true),
    speed(PLAYER_BULLET_SPEED),
    lifetime(0),
    maxLifetime(BULLET_MAX_LIFETIME)
{
}

Bullet::Bullet(Vector2 position, Vector2 direction, int damage, bool isPlayerBullet) :
    position(position),
    active(true),
    damage(damage),
    playerBullet(isPlayerBullet),
    lifetime(0),
    maxLifetime(BULLET_MAX_LIFETIME)
{
    // Normalize the direction vector
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        this->direction.x = direction.x / length;
        this->direction.y = direction.y / length;
    } else {
        this->direction = {0, -1};  // Default direction if no direction given
    }
    
    // Set the speed based on bullet type
    speed = isPlayerBullet ? PLAYER_BULLET_SPEED : ENEMY_BULLET_SPEED;
    
    // Determine visual direction
    visualDirection = calculateVisualDirection(this->direction);
}

Bullet::~Bullet() {
    // No resources to clean up
}

void Bullet::update(float deltaTime) {
    // Update lifetime
    lifetime += deltaTime;
    if (lifetime >= maxLifetime) {
        active = false;
        return;
    }
    
    // Move the bullet
    position.x += direction.x * speed * deltaTime;
    position.y += direction.y * speed * deltaTime;
    
    // Get level offsets from the GameState
    int levelWidth = 24 * 32;  // mapWidth * TILE_SIZE
    int levelHeight = 24 * 32; // mapHeight * TILE_SIZE
    
    // Check if bullet is out of playable area
    if (position.x < 0 || position.x > levelWidth || 
        position.y < 0 || position.y > levelHeight) {
        active = false;
    }
}

void Bullet::draw(Texture2D /*texture*/) {
    if (!active) return;
    
    AssetsManager& assets = AssetsManager::getInstance();
    float zoom = AssetsManager::GetZoom();
    
    // Get sprite based on bullet type
    int bulletType = playerBullet ? 0 : 1;
    Rectangle src = assets.getBulletSprite(bulletType);
    
    // Calculate size based on zoom factor
    float size = BASE_TILE_SIZE * zoom;
    float halfSize = size / 2.0f;
    
    // Draw the bullet
    DrawTexturePro(
        assets.spriteSheet,
        src,
        Rectangle{position.x - halfSize, position.y - halfSize, size, size},
        Vector2{halfSize, halfSize},
        0.0f,
        WHITE
    );
}

Rectangle Bullet::getBounds() const {
    // Return bullet collision bounds
    return Rectangle{
        position.x - 4,
        position.y - 4,
        8,
        8
    };
}

BulletDirection Bullet::calculateVisualDirection(Vector2 dir) {
    // Calculate the angle in radians
    float angle = atan2f(dir.y, dir.x);
    
    // Convert to degrees
    float degrees = angle * 180.0f / PI;
    
    // Normalize to 0-360 range
    if (degrees < 0) {
        degrees += 360.0f;
    }
    
    // Map to 8 directions
    if (degrees >= 337.5f || degrees < 22.5f) {
        return BULLET_RIGHT;
    } else if (degrees >= 22.5f && degrees < 67.5f) {
        return BULLET_DOWN_RIGHT;
    } else if (degrees >= 67.5f && degrees < 112.5f) {
        return BULLET_DOWN;
    } else if (degrees >= 112.5f && degrees < 157.5f) {
        return BULLET_DOWN_LEFT;
    } else if (degrees >= 157.5f && degrees < 202.5f) {
        return BULLET_LEFT;
    } else if (degrees >= 202.5f && degrees < 247.5f) {
        return BULLET_UP_LEFT;
    } else if (degrees >= 247.5f && degrees < 292.5f) {
        return BULLET_UP;
    } else {  // degrees >= 292.5f && degrees < 337.5f
        return BULLET_UP_RIGHT;
    }
}