#pragma once

#include "raylib.h"
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