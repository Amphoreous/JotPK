#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include "raylib.h"
#include "raymath.h"
#include "game/powerup.h"
#include <cmath>

// Common PowerupType mapping constants for compatibility
#define COIN_1 PowerupType::POWERUP_COIN_1
#define COIN_5 PowerupType::POWERUP_COIN_5
#define POWERUP_SPEED PowerupType::POWERUP_BOOT
#define POWERUP_RAPIDFIRE PowerupType::POWERUP_COFFEE
#define POWERUP_SPREAD PowerupType::POWERUP_HAT
#define POWERUP_SHOTGUN PowerupType::POWERUP_GLOVE

// Remove the custom Vector2 helper to avoid conflicts - use raylib's Vector2{ x, y } syntax
// or use raymath.h functions

#endif // GAME_DEFS_H