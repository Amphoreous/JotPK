#pragma once

#include "raylib.h"
#include "game/game_defs.h"
#include <string>
#include <map>

class AssetsManager {
public:
    // Singleton pattern
    static AssetsManager& getInstance() {
        static AssetsManager instance;
        return instance;
    }

    // Prevent copies
    AssetsManager(AssetsManager const&) = delete;
    void operator=(AssetsManager const&) = delete;
    
    // Main methods
    void initialize();
    void unload();
    
    // Sprite selection helpers
    Rectangle getEnemySprite(int enemyType, int direction, int frame);
    Rectangle getPlayerSprite(int direction, int frame);
    Rectangle getBulletSprite(int bulletType);
    Rectangle getPowerupSprite(int powerupType);
    Rectangle getMenuSprite(int spriteIndex);
    Rectangle getUISprite(const char* element);
    
    // Add this new declaration
    Rectangle getPlayerFeetSprite(float animationTimer);
    
    // Add these helper methods
    static float GetZoom() { return PIXEL_ZOOM; }
    static Vector2 GetScreenCenter() {
        return {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    }
    static Vector2 GetGameOffset() {
        // Center a 16x16 tile game board (adjust dimensions as needed)
        int gameWidth = 16 * TILE_SIZE;
        int gameHeight = 16 * TILE_SIZE;
        return {
            (GetScreenWidth() - gameWidth) / 2.0f,
            (GetScreenHeight() - gameHeight) / 2.0f
        };
    }
    
    // Assets
    Texture2D spriteSheet;
    Texture2D backgroundTexture;
    
    // Audio
    Sound soundShoot;
    Sound soundHit;
    Sound soundExplosion;
    Sound soundPowerup;
    Sound soundDamage;
    
    // Music
    Music music;
    
    // Font
    Font gameFont;
    
private:
    AssetsManager() : loaded(false) {}
    bool loaded;
};