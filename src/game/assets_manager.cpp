#include "game/assets_manager.h"
#include <iostream>
#include <cstring>

void AssetsManager::initialize() {
    std::cout << "Working directory: " << GetWorkingDirectory() << std::endl;
    std::cout << "Attempting to load assets.png..." << std::endl;
    
    // Try multiple locations
    const char* paths[] = {
        "assets.png",
        "resources/assets.png",
        "../resources/assets.png",
        "../../resources/assets.png",
        "../assets.png"
    };
    
    bool assetLoaded = false;
    for (const auto& path : paths) {
        std::cout << "Trying path: " << path << std::endl;
        spriteSheet = LoadTexture(path);
        if (spriteSheet.id != 0) {
            std::cout << "Successfully loaded assets from: " << path << std::endl;
            assetLoaded = true;
            break;
        }
    }
    
    if (!assetLoaded) {
        std::cerr << "Failed to load assets.png! Creating a placeholder texture." << std::endl;
        // Create a simple colored placeholder so the game doesn't crash
        Image img = GenImageChecked(256, 256, 32, 32, RED, BLUE);
        spriteSheet = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    
    std::cout << "Asset texture details: ID=" << spriteSheet.id
              << ", Size=" << spriteSheet.width << "x" << spriteSheet.height
              << ", Format=" << spriteSheet.format << std::endl;

    if (!loaded) {
        // Additional resources (commented out until you have them)
        /*
        backgroundTexture = LoadTexture("resources/background.png");
        
        // Load sounds
        soundShoot = LoadSound("resources/audio/shoot.wav");
        soundHit = LoadSound("resources/audio/hit.wav");
        soundExplosion = LoadSound("resources/audio/explosion.wav");
        soundPowerup = LoadSound("resources/audio/powerup.wav");
        soundDamage = LoadSound("resources/audio/damage.wav");
        
        // Load music
        music = LoadMusicStream("resources/audio/music.mp3");
        
        // Load font
        gameFont = LoadFontEx("resources/font.ttf", 32, NULL, 0);
        */
        
        loaded = true;
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
    // Based on the original AbigailGame.cs player sprites
    // Map the 8-direction format to the sprite positions
    int x, y;
    
    // Default positions for player sprite in your tileset
    // You might need to adjust these values based on your actual sprite sheet layout
    switch(direction) {
        case 0: // UP
            x = 336;
            y = 96; 
            break;
        case 1: // UP_RIGHT
            x = 352;
            y = 96;
            break;
        case 2: // RIGHT
            x = 368;
            y = 96;
            break;
        case 3: // DOWN_RIGHT
            x = 384;
            y = 96;
            break;
        case 4: // DOWN
            x = 336;
            y = 112;
            break;
        case 5: // DOWN_LEFT
            x = 352;
            y = 112;
            break;
        case 6: // LEFT
            x = 368;
            y = 112;
            break;
        case 7: // UP_LEFT
            x = 384;
            y = 112;
            break;
        case 8: // IDLE (default to DOWN)
        default:
            x = 336;
            y = 112;
            break;
    }
    
    // For animation frames
    if (direction != 8 && frame > 0) { // Not idle and animated
        x += 16; // Move to next frame in spritesheet
    }
    
    return Rectangle{(float)x, (float)y, 16.0f, 16.0f};
}

// Enemy sprite function based on the original Stardew Valley implementation
Rectangle AssetsManager::getEnemySprite(int enemyType, int direction, int frame) {
    // Enemy sprites based on type and direction
    int baseX = 336;
    int baseY = 128 + (enemyType * 32);
    
    // Calculate position based on direction (4 directions: 0=down, 1=left, 2=right, 3=up)
    int xOffset = (direction % 4) * 16;
    int yOffset = (frame % 2) * 16;  // Alternate between frames for animation
    
    return Rectangle{(float)(baseX + xOffset), (float)(baseY + yOffset), 16.0f, 16.0f};
}

// Bullet sprite function
Rectangle AssetsManager::getBulletSprite(int bulletType) {
    // Bullet sprites based on type
    switch(bulletType) {
        case 0: // Player bullet
            return Rectangle{400.0f, 96.0f, 8.0f, 8.0f};
        case 1: // Enemy bullet
            return Rectangle{408.0f, 96.0f, 8.0f, 8.0f};
        case 2: // Special bullet
            return Rectangle{416.0f, 96.0f, 8.0f, 8.0f};
        default:
            return Rectangle{400.0f, 96.0f, 8.0f, 8.0f};
    }
}

// Powerup sprite function
Rectangle AssetsManager::getPowerupSprite(int powerupType) {
    // Powerup sprites based on type
    switch(powerupType) {
        case 0: // Speed
            return Rectangle{432.0f, 96.0f, 16.0f, 16.0f};
        case 1: // Attack
            return Rectangle{448.0f, 96.0f, 16.0f, 16.0f};
        case 2: // Health
            return Rectangle{464.0f, 96.0f, 16.0f, 16.0f};
        case 3: // Shield
            return Rectangle{480.0f, 96.0f, 16.0f, 16.0f};
        default:
            return Rectangle{432.0f, 96.0f, 16.0f, 16.0f};
    }
}

Rectangle AssetsManager::getMenuSprite(int spriteIndex) {
    // Menu sprites
    return Rectangle{336.0f + (float)(spriteIndex * 16), 80.0f, 16.0f, 16.0f};
}

// New method for UI elements
Rectangle AssetsManager::getUISprite(const char* element) {
    // UI elements like hearts, coins, etc.
    
    if (strcmp(element, "heart") == 0) {
        // Heart is at (400, 1776) in original game
        return Rectangle{272.0f, 128.0f, 16.0f, 16.0f};
    } 
    else if (strcmp(element, "coin") == 0) {
        // Coin is at (272, 1808) in original game
        return Rectangle{144.0f, 160.0f, 16.0f, 16.0f};
    }
    else if (strcmp(element, "ammo") == 0) {
        // Ammo indicator
        return Rectangle{288.0f, 128.0f, 16.0f, 16.0f};
    }
    else if (strcmp(element, "lives_count") == 0) {
        return Rectangle{272.0f, 144.0f, 16.0f, 16.0f};
    }
    
    // Default UI element
    return Rectangle{0.0f, 0.0f, 16.0f, 16.0f};
}