#include "raylib.h"

// This is a utility to create a simple placeholder assets.png file
void CreatePlaceholderAssets() {
    int width = 256;  // 16 sprites wide
    int height = 384;  // 24 sprites high (covers all our needs)
    
    Image img = GenImageColor(width, height, BLACK);
    
    // Player sprites (different colors for different directions)
    for (int dir = 0; dir < 9; dir++) {
        for (int frame = 0; frame < 4; frame++) {
            Color color;
            switch (dir) {
                case 0: color = RED; break;     // UP
                case 1: color = ORANGE; break;  // UP_RIGHT
                case 2: color = YELLOW; break;  // RIGHT
                case 3: color = GREEN; break;   // DOWN_RIGHT
                case 4: color = BLUE; break;    // DOWN
                case 5: color = PURPLE; break;  // DOWN_LEFT
                case 6: color = PINK; break;    // LEFT
                case 7: color = BROWN; break;   // UP_LEFT
                case 8: color = WHITE; break;   // IDLE
                default: color = GRAY; break;
            }
            
            // Draw a colored rectangle for each sprite
            for (int y = 0; y < 16; y++) {
                for (int x = 0; x < 16; x++) {
                    if (x == 0 || y == 0 || x == 15 || y == 15) {
                        // Border
                        ImageDrawPixel(&img, frame * 16 + x, dir * 16 + y, BLACK);
                    } else {
                        // Inside - vary by frame
                        Color frameColor = color;
                        frameColor.a = 128 + frame * 32;
                        ImageDrawPixel(&img, frame * 16 + x, dir * 16 + y, frameColor);
                    }
                }
            }
        }
    }
    
    // Enemy sprites (rows 9-16)
    for (int type = 0; type < 8; type++) {
        for (int frame = 0; frame < 4; frame++) {
            Color color;
            switch (type) {
                case 0: color = RED; break;     // ORC
                case 1: color = WHITE; break;   // GHOST
                case 2: color = GREEN; break;   // OGRE
                case 3: color = YELLOW; break;  // MUMMY
                case 4: color = PURPLE; break;  // DEVIL
                case 5: color = BROWN; break;   // MUSHROOM
                case 6: color = GRAY; break;    // SPIKEY
                case 7: color = BLACK; break;   // BOSS
                default: color = BLUE; break;
            }
            
            // Draw a colored rectangle for each sprite
            for (int y = 0; y < 16; y++) {
                for (int x = 0; x < 16; x++) {
                    if (x == 0 || y == 0 || x == 15 || y == 15) {
                        // Border
                        ImageDrawPixel(&img, frame * 16 + x, (9 + type) * 16 + y, BLACK);
                    } else {
                        // Inside - vary by frame
                        Color frameColor = color;
                        frameColor.a = 128 + frame * 32;
                        ImageDrawPixel(&img, frame * 16 + x, (9 + type) * 16 + y, frameColor);
                    }
                }
            }
        }
    }
    
    // Bullet sprites (row 17-18)
    for (int dir = 0; dir < 8; dir++) {
        // Player bullets (row 17)
        for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 16; x++) {
                if ((x-8)*(x-8) + (y-8)*(y-8) <= 16) {
                    ImageDrawPixel(&img, dir * 16 + x, 17 * 16 + y, YELLOW);
                }
            }
        }
        
        // Enemy bullets (row 18)
        for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 16; x++) {
                if ((x-8)*(x-8) + (y-8)*(y-8) <= 16) {
                    ImageDrawPixel(&img, dir * 16 + x, 18 * 16 + y, RED);
                }
            }
        }
    }
    
    // Powerup sprites (row 19)
    for (int type = 0; type < 9; type++) {
        Color color;
        switch (type) {
            case 0: color = GOLD; break;     // COIN_1
            case 1: color = YELLOW; break;   // COIN_5
            case 2: color = RED; break;      // LIFE
            case 3: color = GRAY; break;     // AMMO
            case 4: color = BLUE; break;     // BOOT
            case 5: color = BROWN; break;    // COFFEE
            case 6: color = GREEN; break;    // HAT
            case 7: color = PURPLE; break;   // GLOVE
            case 8: color = BLACK; break;    // NUKE
            default: color = WHITE; break;
        }
        
        // Draw a colored square for each powerup
        for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 16; x++) {
                if (x == 0 || y == 0 || x == 15 || y == 15) {
                    // Border
                    ImageDrawPixel(&img, type * 16 + x, 19 * 16 + y, BLACK);
                } else {
                    // Inside
                    ImageDrawPixel(&img, type * 16 + x, 19 * 16 + y, color);
                }
            }
        }
    }
    
    // UI elements (row 20)
    // Heart
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            if ((x <= 7 && (x-3)*(x-3) + (y-5)*(y-5) <= 10) ||
                (x > 7 && (x-11)*(x-11) + (y-5)*(y-5) <= 10)) {
                ImageDrawPixel(&img, x, 20 * 16 + y, RED);
            }
        }
    }
    
    // Coin
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            if ((x-8)*(x-8) + (y-8)*(y-8) <= 36) {
                ImageDrawPixel(&img, x + 16, 20 * 16 + y, GOLD);
            }
        }
    }
    
    // Save the image
    ExportImage(img, "resources/assets.png");
    UnloadImage(img);
}