#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include "raylib.h"
#include "game/powerup.h"
#include <cmath>

// Common PowerupType mapping constants for compatibility
#define COIN_1 PowerupType::POWERUP_COIN_1
#define COIN_5 PowerupType::POWERUP_COIN_5
#define POWERUP_SPEED PowerupType::POWERUP_BOOT
#define POWERUP_RAPIDFIRE PowerupType::POWERUP_COFFEE
#define POWERUP_SPREAD PowerupType::POWERUP_HAT
#define POWERUP_SHOTGUN PowerupType::POWERUP_GLOVE

// Remove the custom definitions of Clamp and Vector2Distance 
// (since they're already defined in raymath.h)

// Cast helper to safely convert between int and float
inline Vector2 MakeVector2(float x, float y) {
    Vector2 result;
    result.x = x;
    result.y = y;
    return result;
}

#endif // GAME_DEFS_H