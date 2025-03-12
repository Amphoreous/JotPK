#include "game/player.h"
#include "game/game_defs.h"
#include "game/raylib_wrapper.h"
#include "game/assets_manager.h"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <algorithm>
#include <iostream>

#include "game/vector2_helpers.h"

// Ensure position and targetPosition are declared
Vector2 position;
Vector2 targetPosition;

// If you need to calculate distance, prefer raymath's implementation:
float distance = Vector2Distance(position, targetPosition);

// Or use our wrapper if raymath is not available:
#if !defined(RAYMATH_H)
float distance = Vector2Distance(position, targetPosition);
#endif

// Don't redefine Clamp here - use the one from game_defs.h

// Constants for player movement and shooting
#define PLAYER_BASE_SPEED 150.0f
#define PLAYER_SPEED_BOOST 1.5f
#define PLAYER_BASE_SHOOT_DELAY 200.0f
#define PLAYER_RAPID_FIRE_FACTOR 0.5f
#define PLAYER_ANIMATION_SPEED 0.2f
#define PLAYER_FRAME_COUNT 4
#define PLAYER_FOOTSTEP_DELAY 200.0f
#define PLAYER_MOTION_DELAY 100.0f

Player::Player() :
    position({0, 0}),
    direction(IDLE),
    animationTimer(0),
    currentFrame(0),
    baseMovementSpeed(PLAYER_BASE_SPEED),
    movementSpeed(PLAYER_BASE_SPEED),
    lastMoveWasKeyboard(true),
    shootTimer(0),
    baseShootDelay(PLAYER_BASE_SHOOT_DELAY),
    invincibilityTimer(0),
    currentShootDirection({0.0f, -1.0f}) // Inicialización estilo C++11
{
}

Player::~Player() {
    // Clean up any resources if needed
}

void Player::Initialize(Vector2 startPosition) {
    position = startPosition;
    direction = IDLE;
    animationTimer = 0;
    currentFrame = 0;
    movementSpeed = baseMovementSpeed;
    shootTimer = 0;
    invincibilityTimer = 0;
    activePowerups.clear();
}

void Player::Update(float deltaTime) {
    // Update timers
    if (shootTimer > 0) {
        shootTimer -= deltaTime;
    }
    
    if (invincibilityTimer > 0) {
        invincibilityTimer -= deltaTime;
    }
    
    // Update powerups
    for (auto it = activePowerups.begin(); it != activePowerups.end();) {
        it->second -= deltaTime;
        if (it->second <= 0) {
            // Deactivate powerup
            DeactivatePowerup(it->first);
            it = activePowerups.erase(it);
        } else {
            ++it;
        }
    }
    
    // Handle input
    HandleKeyboardInput(deltaTime);
    
    // Check if gamepad is available
    if (IsGamepadAvailable(0)) {
        HandleGamepadInput(deltaTime);
    }
    
    // Update animation
    UpdateAnimation(deltaTime);
}

void Player::HandleKeyboardInput(float deltaTime) {
    Vector2 moveDirection = {0, 0};
    
    // Movement input - WASD
    if (IsKeyDown(KEY_W)) {
        moveDirection.y = -1;
    }
    if (IsKeyDown(KEY_S)) {
        moveDirection.y = 1;
    }
    if (IsKeyDown(KEY_A)) {
        moveDirection.x = -1;
    }
    if (IsKeyDown(KEY_D)) {
        moveDirection.x = 1;
    }
    
    // If there is movement input, move the player
    if (moveDirection.x != 0 || moveDirection.y != 0) {
        lastMoveWasKeyboard = true;
        Move(moveDirection, deltaTime);
    }
    
    // Handle shooting with arrow keys
    bool shooting = false;
    Vector2 shootDirection = {0, 0};
    
    if (IsKeyDown(KEY_UP)) {
        shootDirection.y = -1;
        shooting = true;
    }
    if (IsKeyDown(KEY_DOWN)) {
        shootDirection.y = 1;
        shooting = true;
    }
    if (IsKeyDown(KEY_LEFT)) {
        shootDirection.x = -1;
        shooting = true;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        shootDirection.x = 1;
        shooting = true;
    }
    
    // Also allow shooting with space key in the direction the player is facing
    if (IsKeyDown(KEY_SPACE)) {
        shooting = true;
        shootDirection = GetShootingDirection();
    }
    
    // Process shooting
    if (shooting && CanShoot()) {
        // Normalize the shoot direction
        float length = sqrt(shootDirection.x * shootDirection.x + shootDirection.y * shootDirection.y);
        if (length > 0) {
            shootDirection.x /= length;
            shootDirection.y /= length;
        } else {
            // Default to shooting upward if no direction given
            shootDirection.y = -1;
        }
        
        // Set the shoot direction for retrieval by the game manager
        currentShootDirection = shootDirection;
        justFired = true;
        
        // Reset the shoot timer
        ResetShootTimer();
    }
}

void Player::HandleGamepadInput(float deltaTime) {
    Vector2 leftStick = {
        GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X),
        GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y)
    };
    
    // Use individual component assignment
    currentShootDirection.x = leftStick.x;
    currentShootDirection.y = leftStick.y;
    
    // Apply deadzone
    float deadZone = 0.25f;
    if (fabsf(leftStick.x) < deadZone) leftStick.x = 0;
    if (fabsf(leftStick.y) < deadZone) leftStick.y = 0;
    
    // If there is movement input on gamepad
    if (leftStick.x != 0 || leftStick.y != 0) {
        lastMoveWasKeyboard = false;
        Move(leftStick, deltaTime);
    }
}

void Player::Move(Vector2 moveDir, float deltaTime) {
    if (moveDir.x != 0 || moveDir.y != 0) {
        float length = sqrtf(moveDir.x * moveDir.x + moveDir.y * moveDir.y);
        if (length > 0) {
            // Normalize movement vector
            float normalizedX = moveDir.x / length;
            float normalizedY = moveDir.y / length;
            
            // Update position using normalized direction
            position.x += normalizedX * movementSpeed * deltaTime;
            position.y += normalizedY * movementSpeed * deltaTime;
            
            // Convert to player direction enum
            if (moveDir.x == 0 && moveDir.y < 0) {
                direction = UP;
            } else if (moveDir.x > 0 && moveDir.y < 0) {
                direction = UP_RIGHT;
            } else if (moveDir.x > 0 && moveDir.y == 0) {
                direction = RIGHT;
            } else if (moveDir.x > 0 && moveDir.y > 0) {
                direction = DOWN_RIGHT;
            } else if (moveDir.x == 0 && moveDir.y > 0) {
                direction = DOWN;
            } else if (moveDir.x < 0 && moveDir.y > 0) {
                direction = DOWN_LEFT;
            } else if (moveDir.x < 0 && moveDir.y == 0) {
                direction = LEFT;
            } else if (moveDir.x < 0 && moveDir.y < 0) {
                direction = UP_LEFT;
            }
        }
    } else {
        direction = IDLE;
    }
    
    // Keep player within screen bounds - use raymath's Clamp function
    Vector2 levelSize = {768.0f, 768.0f}; // Level size in pixels
    position.x = Clamp(position.x, 16.0f, levelSize.x - 16.0f);
    position.y = Clamp(position.y, 16.0f, levelSize.y - 16.0f);
}

void Player::UpdateAnimation(float deltaTime) {
    // Only animate when moving
    if (direction != IDLE) {
        animationTimer += deltaTime;
        if (animationTimer >= PLAYER_ANIMATION_SPEED) {
            animationTimer = 0;
            currentFrame = (currentFrame + 1) % 2; // Toggle between 0 and 1 for basic animation
        }
    } else {
        // Reset animation frame for idle state
        currentFrame = 0;
        animationTimer = 0;
    }
}

void Player::Draw(Texture2D* playerTextures) {
    AssetsManager& assets = AssetsManager::getInstance();
    
    // Solo renderizar pies si el jugador se está moviendo
    if (direction != IDLE) {
        // Dibujar pies primero (están detrás del cuerpo)
        Rectangle feetSrc = assets.getPlayerFeetSprite(animationTimer);
        DrawTexturePro(
            assets.spriteSheet,
            feetSrc,
            Rectangle{position.x + 4.0f, position.y + 13.0f, 10.0f, 3.0f},
            Vector2{0.0f, 0.0f},
            0.0f,
            WHITE
        );
    }
    
    // Convertir el sistema de 8 direcciones a 4 direcciones para el renderizado
    int facingDirection;
    if (direction == UP || direction == UP_RIGHT || direction == UP_LEFT) facingDirection = 0;
    else if (direction == RIGHT || direction == DOWN_RIGHT) facingDirection = 1;
    else if (direction == DOWN || direction == DOWN_LEFT) facingDirection = 2;
    else facingDirection = 3; // LEFT
    
    // Dibujar el cuerpo del jugador
    Rectangle src = assets.getPlayerSprite(facingDirection, 0);
    DrawTexturePro(
        assets.spriteSheet,
        src,
        Rectangle{position.x, position.y, 16.0f, 16.0f},
        Vector2{0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

Rectangle Player::GetBounds() const {
    // Return player collision bounds (slightly smaller than visual size)
    return Rectangle{
        position.x - 12,
        position.y - 12,
        24,
        24
    };
}

Vector2 Player::GetShootingDirection() const {
    // Return normalized vector based on player direction
    switch (direction) {
        case UP:        return {0, -1};
        case UP_RIGHT:  return {0.7071f, -0.7071f};  // normalized (1, -1)
        case RIGHT:     return {1, 0};
        case DOWN_RIGHT:return {0.7071f, 0.7071f};   // normalized (1, 1)
        case DOWN:      return {0, 1};
        case DOWN_LEFT: return {-0.7071f, 0.7071f};  // normalized (-1, 1)
        case LEFT:      return {-1, 0};
        case UP_LEFT:   return {-0.7071f, -0.7071f}; // normalized (-1, -1)
        case IDLE:      return {0, -1};  // Default direction when idle is up
    }
    
    return {0, -1};  // Default fallback
}

bool Player::HasPowerup(int type) const {
    for (const auto& powerup : activePowerups) {
        if (powerup.first == type) {
            return true;
        }
    }
    return false;
}

float Player::GetPowerupTimeRemaining(int type) const {
    for (const auto& powerup : activePowerups) {
        if (powerup.first == type) {
            return powerup.second;
        }
    }
    return 0.0f;
}

void Player::ActivatePowerup(int type, float duration) {
    // Check if powerup is already active
    for (auto& powerup : activePowerups) {
        if (powerup.first == type) {
            // Just update the duration
            powerup.second = std::max(powerup.second, duration);
            return;
        }
    }
    
    // Add new powerup
    activePowerups.push_back({type, duration});
    
    // Apply powerup effect
    switch (type) {
        case POWERUP_SPEED:
            movementSpeed = baseMovementSpeed * PLAYER_SPEED_BOOST;
            break;
        // Other powerups don't directly modify player state
    }
}

void Player::DeactivatePowerup(int type) {
    // Remove powerup effects
    switch (type) {
        case POWERUP_SPEED:
            movementSpeed = baseMovementSpeed;
            break;
        // Other powerup deactivation effects
    }
}

float Player::GetMovementSpeed() const {
    return movementSpeed;
}

float Player::GetShootDelay() const {
    // Apply rapid fire powerup if active
    if (HasPowerup(POWERUP_RAPIDFIRE)) {
        return baseShootDelay * PLAYER_RAPID_FIRE_FACTOR;
    }
    return baseShootDelay;
}

int Player::GetBulletCount() const {
    // Return number of bullets to fire based on active powerups
    if (HasPowerup(POWERUP_SHOTGUN)) {
        return 5;  // 5-way shotgun spread
    } else if (HasPowerup(POWERUP_SPREAD)) {
        return 3;  // 3-way spread
    }
    return 1;  // Default - single bullet
}

int Player::GetBulletDamage() const {
    // Base damage is 1, could be modified by powerups
    return 1;
}

// Implementation of GetAnimationFrame (missing in the header)
int Player::GetAnimationFrame() const {
    return currentFrame;
}

// Remove or comment out the duplicate definition of MakeVector2
/*
Vector2 MakeVector2(float x, float y) {
    Vector2 result = {x, y};
    return result;
}
*/

// Asegúrate de que las funciones que estás llamando no tengan sobrecargas ambiguas
// Revisa las líneas 231 y 232 y asegúrate de que los parámetros sean correctos
