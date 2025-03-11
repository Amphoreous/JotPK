#include "game/assets_manager.h"
#include <iostream>

void AssetsManager::initialize() {
    // Load the main sprite sheet
    spriteSheet = LoadTexture("assets.png");
    
    if (spriteSheet.id == 0) {
        std::cerr << "Failed to load assets.png" << std::endl;
    } else {
        std::cout << "Assets loaded successfully. Size: " << 
            spriteSheet.width << "x" << spriteSheet.height << std::endl;
    }
}

void AssetsManager::unload() {
    UnloadTexture(spriteSheet);
}

Rectangle AssetsManager::getPlayerSprite(int direction, int frame) {
    // Player sprites are in the first row
    // Each animation has 4 frames and there are 9 directions
    int x = frame * 16;
    int y = direction * 16;
    return Rectangle{(float)x, (float)y, 16, 16};
}

Rectangle AssetsManager::getEnemySprite(int enemyType, int frame) {
    // Enemy sprites start at row 9
    int x = frame * 16;
    int y = (9 + enemyType) * 16;
    return Rectangle{(float)x, (float)y, 16, 16};
}

Rectangle AssetsManager::getBulletSprite(int direction, bool isPlayerBullet) {
    // Bullets are in row 17
    // Player bullets in first row, enemy bullets in second
    int x = direction * 16;
    int y = 17 * 16 + (isPlayerBullet ? 0 : 16);
    return Rectangle{(float)x, (float)y, 16, 16};
}

Rectangle AssetsManager::getPowerupSprite(int type) {
    // Powerups are in row 19
    int x = type * 16;
    int y = 19 * 16;
    return Rectangle{(float)x, (float)y, 16, 16};
}

Rectangle AssetsManager::getUISprite(const char* element) {
    // UI elements are in row 20+
    // This is simplified - in a real implementation, you'd map element names to coordinates
    if (strcmp(element, "heart") == 0) {
        return Rectangle{0, 20 * 16, 16, 16};
    } else if (strcmp(element, "coin") == 0) {
        return Rectangle{16, 20 * 16, 16, 16};
    }
    
    // Default UI element (empty)
    return Rectangle{0, 0, 16, 16};
}