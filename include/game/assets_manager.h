#ifndef ASSETS_MANAGER_H
#define ASSETS_MANAGER_H

#include "raylib.h"

// Singleton class to manage all game assets
class AssetsManager {
public:
    static AssetsManager& getInstance() {
        static AssetsManager instance;
        return instance;
    }
    
    void initialize();
    void unload();
    
    // Main sprite sheet
    Texture2D spriteSheet;
    
    // Get specific sprite regions
    Rectangle getPlayerSprite(int direction, int frame);
    Rectangle getEnemySprite(int enemyType, int frame);
    Rectangle getBulletSprite(int direction, bool isPlayerBullet);
    Rectangle getPowerupSprite(int type);
    Rectangle getUISprite(const char* element);
    
private:
    AssetsManager() = default;
    ~AssetsManager() = default;
    AssetsManager(const AssetsManager&) = delete;
    AssetsManager& operator=(const AssetsManager&) = delete;
};

#endif // ASSETS_MANAGER_H