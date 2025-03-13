#include "game/player.h"
#include "game/game_defs.h"
#include "game/assets_manager.h"
#include "game/level.h"
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <algorithm>

// Constants
#define PLAYER_BASE_SPEED 3.0f
#define PLAYER_FRAME_COUNT 4
#define PLAYER_ANIMATION_CYCLE 400.0f // 400ms per animation cycle
#define PLAYER_SIZE 16
#define PLAYER_COLLISION_OFFSET 4
#define PLAYER_RUN_SPEED_MULTIPLIER 1.25f
#define PLAYER_ZOMBIE_SPEED_MULTIPLIER 1.5f
#define PLAYER_POWERUP_SPEED_MULTIPLIER 1.5f
#define TILE_SIZE 48.0f
#define PLAYER_COLLISION_OFFSET 4.0f
#define COLLISION_OFFSET 4.0f

Player::Player() :
    position({0, 0}),
    animationTimer(0),
    currentFrame(0),
    movementSpeed(PLAYER_BASE_SPEED),
    direction(Direction::IDLE),
    runSpeedLevel(0),
    zombieModeTimer(0),
    activePowerups(),
    justFired(false),
    shootDirection({0, -1}),
    bulletDamage(1),
    bulletCount(1),
    shouldDraw(true),
    shotTimer(0),
    shootingDelay(300.0f),
    fireSpeedLevel(0),
    shootingDirections()
{
}

void Player::Initialize(Vector2 startPosition) {
    position = startPosition;
    animationTimer = 0;
    currentFrame = 0;
    movementSpeed = PLAYER_BASE_SPEED;
    direction = IDLE;
}

void Player::Update(float deltaTime, Level* level) {
    HandleInput(deltaTime, level);
    UpdatePowerups(deltaTime);
    
    // Update shot timer
    if (shotTimer > 0) {
        shotTimer -= deltaTime * 1000.0f;
    }
    
    // Shooting input check based on arrow keys
    if (IsKeyDown(KEY_UP)) {
        addPlayerShootingDirection(Direction::UP);
    }
    if (IsKeyDown(KEY_DOWN)) {
        addPlayerShootingDirection(Direction::DOWN);
    }
    if (IsKeyDown(KEY_LEFT)) {
        addPlayerShootingDirection(Direction::LEFT);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        addPlayerShootingDirection(Direction::RIGHT);
    }

    // Clear shooting directions when keys released
    if (!IsKeyDown(KEY_UP)) shootingDirections.erase(Direction::UP);
    if (!IsKeyDown(KEY_DOWN)) shootingDirections.erase(Direction::DOWN);
    if (!IsKeyDown(KEY_LEFT)) shootingDirections.erase(Direction::LEFT);
    if (!IsKeyDown(KEY_RIGHT)) shootingDirections.erase(Direction::RIGHT);

    // Handle shooting if direction pressed and shot timer elapsed
    if (!shootingDirections.empty() && shotTimer <= 0) {
        // Calculate shooting delay with powerup modifiers
        float shootDelay = shootingDelay;
        
        if (activePowerups.find(3) != activePowerups.end()) {
            shootDelay /= 4;
        }

        for (int i = 0; i < fireSpeedLevel; i++) {
            shootDelay = shootDelay * 3 / 4;
        }

        if (activePowerups.find(7) != activePowerups.end()) {
            shootDelay = shootDelay * 3 / 2;
        }

        shootDelay = std::max(shootDelay, 20.0f);

        // Handle different shooting patterns
        if (activePowerups.find(2) != activePowerups.end()) {
            SpawnBulletsInAllDirections();
        } else {
            SpawnBullet(*shootingDirections.begin());
        }

        shotTimer = shootDelay;
    }
}

void Player::HandleInput(float deltaTime, Level* level) {
    Vector2 moveDirection = {0, 0};
    
    // Movement input using WASD
    if (IsKeyDown(KEY_W)) moveDirection.y = -1;
    if (IsKeyDown(KEY_S)) moveDirection.y = 1;
    if (IsKeyDown(KEY_A)) moveDirection.x = -1;
    if (IsKeyDown(KEY_D)) moveDirection.x = 1;
    
    // Move player if there's input
    if (moveDirection.x != 0 || moveDirection.y != 0) {
        Move(moveDirection, deltaTime, level);
    } else {
        direction = IDLE;
        currentFrame = 0;
        animationTimer = 0;
    }
}

void Player::Move(Vector2 moveDir, float deltaTime, Level* level) {
    // Normalize movement vector
    float length = sqrtf(moveDir.x * moveDir.x + moveDir.y * moveDir.y);
    if (length > 0) {
        moveDir.x /= length;
        moveDir.y /= length;
        
        // Calculate effective speed with all multipliers
        float speed = movementSpeed;
        
        // Apply powerup speed bonus
        if (activePowerups.find(6) != activePowerups.end()) {
            speed *= PLAYER_POWERUP_SPEED_MULTIPLIER;
        }
        
        // Apply zombie mode speed bonus
        if (zombieModeTimer > 0) {
            speed *= PLAYER_ZOMBIE_SPEED_MULTIPLIER;
            zombieModeTimer -= deltaTime;
        }
        
        // Apply run speed level bonus
        for (int i = 0; i < runSpeedLevel; i++) {
            speed *= PLAYER_RUN_SPEED_MULTIPLIER;
        }
        
        // Calculate new position
        Vector2 newPosition = position;
        newPosition.x += moveDir.x * speed * deltaTime;
        newPosition.y += moveDir.y * speed * deltaTime;
        
        // Check collision
        Rectangle playerCollisionBox = {
            newPosition.x + COLLISION_OFFSET,
            newPosition.y + COLLISION_OFFSET,
            PLAYER_SIZE - COLLISION_OFFSET * 2,
            PLAYER_SIZE - COLLISION_OFFSET * 2
        };
        
        if (!IsCollidingWithMap(playerCollisionBox, level)) {
            position = newPosition;
        }
        
        // Update animation timer
        animationTimer += deltaTime * 1000.0f; // Convert to milliseconds
        if (animationTimer >= PLAYER_ANIMATION_CYCLE) {
            animationTimer = 0;
        }
        
        // Update direction based on movement
        UpdateDirection(moveDir);
    }
    
    // Keep player within bounds
    position.x = Clamp(position.x, 16.0f, (level->getWidth() * TILE_SIZE) - 16.0f);
    position.y = Clamp(position.y, 16.0f, (level->getHeight() * TILE_SIZE) - 16.0f);
}

void Player::Draw(Texture2D* optionalTexture) {
    if (!shouldDraw) return;
    
    AssetsManager& assets = AssetsManager::getInstance();
    float zoom = AssetsManager::GetZoom();
    
    // Calculate size based on zoom factor (same as level tiles)
    float size = BASE_TILE_SIZE * zoom;  // This matches the TILE_SIZE calculation
    float halfSize = size / 2.0f;
    
    // Get sprite based on current direction and frame
    Rectangle src = assets.getPlayerSprite(direction, currentFrame);
    
    // Draw the player with proper scaling
    DrawTexturePro(
        assets.spriteSheet,
        src,
        Rectangle{
            position.x - halfSize,  // Center the sprite
            position.y - halfSize,
            size,                   // Use zoomed size
            size
        },
        Vector2{halfSize, halfSize},
        0.0f,
        WHITE
    );

    // Draw feet animation if moving
    if (direction != Direction::IDLE) {
        Rectangle feetSrc = assets.getPlayerFeetSprite(animationTimer);
        DrawTexturePro(
            assets.spriteSheet,
            feetSrc,
            Rectangle{
                position.x - halfSize,
                position.y + halfSize - (4 * zoom),  // Adjust feet position
                size,
                size / 4.0f                          // Feet are 1/4 height of player
            },
            Vector2{halfSize, size / 8.0f},         // Adjust anchor point for feet
            0.0f,
            WHITE
        );
    }
}

void Player::UpdateDirection(Vector2 moveDir) {
    // Convert movement vector to direction enum
    if (moveDir.x == 0 && moveDir.y < 0) direction = UP;
    else if (moveDir.x > 0 && moveDir.y < 0) direction = UP_RIGHT;
    else if (moveDir.x > 0 && moveDir.y == 0) direction = RIGHT;
    else if (moveDir.x > 0 && moveDir.y > 0) direction = DOWN_RIGHT;
    else if (moveDir.x == 0 && moveDir.y > 0) direction = DOWN;
    else if (moveDir.x < 0 && moveDir.y > 0) direction = DOWN_LEFT;
    else if (moveDir.x < 0 && moveDir.y == 0) direction = LEFT;
    else if (moveDir.x < 0 && moveDir.y < 0) direction = UP_LEFT;
}

bool Player::HasPowerup(int type) const {
    // Simple powerup check
    return false; // Implement powerup system as needed
}

// Update IsCollidingWithMap function
bool Player::IsCollidingWithMap(Rectangle playerBox, Level* level) {
    if (!level) return false;

    // Convert player position to tile coordinates
    int tileStartX = static_cast<int>((playerBox.x) / TILE_SIZE);
    int tileStartY = static_cast<int>((playerBox.y) / TILE_SIZE);
    int tileEndX = static_cast<int>((playerBox.x + playerBox.width) / TILE_SIZE);
    int tileEndY = static_cast<int>((playerBox.y + playerBox.height) / TILE_SIZE);

    // Check each tile the player might be colliding with
    for (int y = tileStartY; y <= tileEndY; y++) {
        for (int x = tileStartX; x <= tileEndX; x++) {
            // Check if tile coordinates are valid
            if (x >= 0 && x < level->getWidth() && y >= 0 && y < level->getHeight()) {
                // Check if tile is not passable (barriers, cacti, fences)
                if (!level->isPassable(x * TILE_SIZE, y * TILE_SIZE)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void Player::AddPowerup(int type, float duration) {
    activePowerups[type] = duration;
}

void Player::UpdatePowerups(float deltaTime) {
    std::vector<int> expiredPowerups;
    
    for (auto& powerup : activePowerups) {
        powerup.second -= deltaTime;
        if (powerup.second <= 0) {
            expiredPowerups.push_back(powerup.first);
        }
    }
    
    for (int type : expiredPowerups) {
        activePowerups.erase(type);
    }
}

void Player::SetRunSpeedLevel(int level) {
    runSpeedLevel = level;
}

void Player::SetZombieMode(float duration) {
    zombieModeTimer = duration;
}

Rectangle Player::GetBounds() const {
    return Rectangle{
        position.x + PLAYER_COLLISION_OFFSET,
        position.y + PLAYER_COLLISION_OFFSET,
        PLAYER_SIZE - PLAYER_COLLISION_OFFSET * 2,
        PLAYER_SIZE - PLAYER_COLLISION_OFFSET * 2
    };
}

void Player::ActivatePowerup(int type) {
    // Implement powerup activation logic
    switch(type) {
        case 2: // POWERUP_SPREAD
            bulletCount = 3;
            break;
        case 3: // POWERUP_RAPIDFIRE
            bulletDamage *= 2;
            break;
        case 7: // POWERUP_SHOTGUN
            bulletCount = 5;
            break;
        // Add other powerup cases
    }
}

void Player::SpawnBullet(Direction dir) {
    justFired = true;
    
    Vector2 shootDir;
    switch(dir) {
        case UP: shootDir = {0, -1}; break;
        case DOWN: shootDir = {0, 1}; break;
        case LEFT: shootDir = {-1, 0}; break;
        case RIGHT: shootDir = {1, 0}; break;
    }
    shootDirection = shootDir;
}

void Player::SpawnBulletsInAllDirections() {
    justFired = true;
    // Main directions
    SpawnBullet(UP);
    SpawnBullet(RIGHT); 
    SpawnBullet(DOWN);
    SpawnBullet(LEFT);
}

void Player::addPlayerShootingDirection(Direction dir) {
    shootingDirections.insert(dir);
}
