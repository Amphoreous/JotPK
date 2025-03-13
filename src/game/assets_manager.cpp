#include "game/assets_manager.h"
#include <iostream>
#include <cstring>
#include "game/game_defs.h"

#define PLAYER_ANIMATION_SPEED 0.2f

void AssetsManager::initialize() {
    spriteSheet = LoadTexture("assets.png");
    
    // Validate texture loaded correctly
    if (spriteSheet.id == 0) {
        std::cerr << "Failed to load assets.png!" << std::endl;
        return;
    }
}

void AssetsManager::unload() {
    if (spriteSheet.id != 0) {
        UnloadTexture(spriteSheet);
    }

    if (loaded) {
        // Unload additional resources when they're added
        /*
        UnloadTexture(backgroundTexture);
        
        UnloadSound(soundShoot);
        UnloadSound(soundHit);
        UnloadSound(soundExplosion);
        UnloadSound(soundPowerup);
        UnloadSound(soundDamage);
        
        UnloadMusicStream(music);
        UnloadFont(gameFont);
        */
        
        loaded = false;
    }
}

Rectangle AssetsManager::getPlayerSprite(int direction, int frame) {
    // Base coordinates from AbigailGame.cs sprite sheet
    const int SPRITE_SIZE = 16;
    const int ROW_OFFSET = 96; // Base Y coordinate for player sprites
    
    int x = 336; // Base X coordinate
    int y = ROW_OFFSET;
    
    switch(static_cast<Direction>(direction)) {
        case UP:
            y = ROW_OFFSET;
            x = 336; // First frame in animation
            break;
        case DOWN:
            y = ROW_OFFSET;
            x = 336 + SPRITE_SIZE; // Second frame
            break;
        case LEFT:
            y = ROW_OFFSET;
            x = 336 + SPRITE_SIZE * 2; // Third frame
            break;
        case RIGHT:
            y = ROW_OFFSET;
            x = 336 + SPRITE_SIZE * 3; // Fourth frame
            break;
        default: // IDLE
            y = 112;
            x = 368; // Use first frame for idle
            break;
    }

    return Rectangle{
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(SPRITE_SIZE),
        static_cast<float>(SPRITE_SIZE)
    };
}

// También necesitamos actualizar la función para los pies
Rectangle AssetsManager::getPlayerFeetSprite(float animationTimer) {
    const int BASE_X = 352;
    const int BASE_Y = 112;
    const int FRAME_WIDTH = 16;
    const int FRAME_HEIGHT = 3;
    
    // Calculate frame based on animation cycle (400ms total, 4 frames)
    int frame = ((int)(animationTimer / 100.0f)) % 4;
    
    return Rectangle{
        static_cast<float>(BASE_X),  
        static_cast<float>(BASE_Y  + frame * FRAME_WIDTH),
        static_cast<float>(FRAME_WIDTH),
        static_cast<float>(FRAME_HEIGHT)
    };
}

// Enemy sprite function based on accurate sprite sheet coordinates
Rectangle AssetsManager::getEnemySprite(int enemyType, int direction, int frame) {
    // Calculate frame offset
    int frameOffset = frame * 16;

    // Calculate direction offset
    switch (enemyType) {
        case 0: // ENEMY_ORC
            return Rectangle{
                static_cast<float>(352 + direction * 16 - 128),
                static_cast<float>(1696 + frameOffset - 1648),
                16.0f,
                16.0f
            };
        case 1: // ENEMY_GHOST
            return Rectangle{
                static_cast<float>(352 + direction * 16 - 128),
                static_cast<float>(1712 + frameOffset - 1648),
                16.0f,
                16.0f
            };
        case 2: // ENEMY_OGRE
            return Rectangle{
                static_cast<float>(384 + direction * 16 - 128),
                static_cast<float>(1696 + frameOffset - 1648),
                16.0f,
                16.0f
            };
        case 3: // ENEMY_MUMMY
            return Rectangle{
                static_cast<float>(384 + direction * 16 - 128),
                static_cast<float>(1712 + frameOffset - 1648),
                16.0f,
                16.0f
            };
        case 4: // ENEMY_DEVIL
            return Rectangle{
                static_cast<float>(416 + direction * 16 - 128),
                static_cast<float>(1696 + frameOffset - 1648),
                16.0f,
                16.0f
            };
        case 5: // ENEMY_MUSHROOM
            return Rectangle{
                static_cast<float>(416 + direction * 16 - 128),
                static_cast<float>(1712 + frameOffset - 1648),
                16.0f,
                16.0f
            };
        case 6: // ENEMY_SPIKEY
            return Rectangle{
                static_cast<float>(448 + direction * 16 - 128),
                static_cast<float>(1696 + frameOffset - 1648),
                16.0f,
                16.0f
            };
        case 7: // ENEMY_BOSS_COWBOY
            return Rectangle{
                static_cast<float>(496 - 128),
                static_cast<float>(1696 - 1648),
                16.0f,
                16.0f
            };
        case 8: // ENEMY_BOSS_FECTOR
        default:
            return Rectangle{
                static_cast<float>(464 - 128),
                static_cast<float>(1696 - 1648),
                16.0f,
                16.0f
            };
    }
}

// Bullet sprite function
Rectangle AssetsManager::getBulletSprite(int bulletType) {
    switch(bulletType) {
        case 0: // Player bullet
            return Rectangle{
                 518 - 128,
                 1760 - 1648,
                 BASE_TILE_SIZE * 0.25f, // 4.0f
                 BASE_TILE_SIZE * 0.25f  // 4.0f
            };
        case 1: // Enemy bullet
            return Rectangle{
                 523 - 128,
                 1760 - 1648,
                 BASE_TILE_SIZE * 0.3125f, // 5.0f
                 BASE_TILE_SIZE * 0.3125f  // 5.0f
            };
        default:
            return Rectangle{
                 518 - 128,
                 1760 - 1648,
                 BASE_TILE_SIZE * 0.25f, // 4.0f
                 BASE_TILE_SIZE * 0.25f  // 4.0f
            };
    }
}

// Powerup sprite function
Rectangle AssetsManager::getPowerupSprite(int powerupType) {
    switch(powerupType) {
        case 0: // COIN_1
            return Rectangle{
                272 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
        case 1: // COIN_5
            return Rectangle{
                288 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
        case 2: // POWERUP_SPREAD (HAT)
            return Rectangle{
                304 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
        case 3: // POWERUP_RAPIDFIRE (COFFEE)
            return Rectangle{
                320 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
        case 4: // POWERUP_NUKE
            return Rectangle{
                336 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
        case 5: // POWERUP_ZOMBIE
            return Rectangle{
                352 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
        case 6: // POWERUP_SPEED (BOOT)
            return Rectangle{
                368 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
        case 7: // POWERUP_SHOTGUN (GLOVE)
            return Rectangle{
                384 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
        case 8: // POWERUP_LIFE
            return Rectangle{
                400 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
        default:
            return Rectangle{
                272 - 128,
                1808 - 1648,
                16.0f,
                16.0f
            };
    }
}

Rectangle AssetsManager::getMenuSprite(int spriteIndex) {
    switch(spriteIndex) {
        case 0: // Title
            return Rectangle{128 - 128, 1744 - 1648, 96, 56}; // (0, 96)
        case 1: // Back button
            return Rectangle{294 - 128, 1782 - 1648, 22, 22}; // (166, 134)
        case 2: // Controls
            return Rectangle{352 - 128, 1648 - 1648, 80, 48}; // (224, 0)
        default:
            return Rectangle{128 - 128, 1744 - 1648, 96, 56}; // (0, 96)
    }
}

// New method for UI elements
Rectangle AssetsManager::getUISprite(const char* element) {
    if (strcmp(element, "heart") == 0) {
        return Rectangle{400 - 128, 1776 - 1648, 16.0f, 16.0f};
    } else if (strcmp(element, "coin") == 0) {
        return Rectangle{272 - 128, 1808 - 1648, 16.0f, 16.0f};
    } else if (strcmp(element, "ammo") == 0) {
        return Rectangle{416 - 128, 1776 - 1648, 16.0f, 16.0f};
    } else if (strcmp(element, "lives_count") == 0) {
        return Rectangle{400 - 128, 1776 - 1648, 16.0f, 16.0f};
    } else {
        // Default UI element
        return Rectangle{400 - 128, 1776 - 1648, 16.0f, 16.0f};
    }
}