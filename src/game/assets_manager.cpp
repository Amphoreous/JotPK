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
    // Convert direction to sprite coordinates
    switch(static_cast<Direction>(direction)) {
        case UP:
            return Rectangle{336.0f, 96.0f, 16.0f, 16.0f};
        case RIGHT:
            return Rectangle{352.0f, 96.0f, 16.0f, 16.0f};
        case DOWN:
            return Rectangle{368.0f, 96.0f, 16.0f, 16.0f};
        case LEFT:
            return Rectangle{384.0f, 96.0f, 16.0f, 16.0f};
        default: // IDLE
            return Rectangle{368.0f, 112.0f, 16.0f, 16.0f};
    }
}

// También necesitamos actualizar la función para los pies
Rectangle AssetsManager::getPlayerFeetSprite(float animationTimer) {
    // Calcula el frame basado en animationTimer % 400f como en el original
    int frameOffset = ((int)(animationTimer / 100.0f) % 4) * 3;
    
    return Rectangle{
        351.0f, 
        112.0f + frameOffset,
        10.0f,
        3.0f
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