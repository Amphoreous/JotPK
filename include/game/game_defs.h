#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include "raylib.h"

// Display/Scaling constants
#define PIXEL_ZOOM 3.0f
#define BASE_TILE_SIZE 16
#define TILE_SIZE (int)(BASE_TILE_SIZE * PIXEL_ZOOM)  // 48 pixels

// Game states
enum GameState {
    START_MENU,
    PLAYING,
    DIED,
    BETWEEN_WAVES,
    SHOPPING,
    GAME_OVER,
    END_CUTSCENE
};

enum Direction {
    UP = 0,
    UP_RIGHT,
    RIGHT,
    DOWN_RIGHT,
    DOWN,
    DOWN_LEFT, 
    LEFT,
    UP_LEFT,
    IDLE
};

// Powerup types
enum PowerupType {
    COIN_1 = 0,
    COIN_5,
    POWERUP_SPREAD,  // HAT
    POWERUP_RAPIDFIRE,  // COFFEE
    POWERUP_NUKE,
    POWERUP_ZOMBIE,
    POWERUP_SPEED,  // BOOT
    POWERUP_SHOTGUN,  // GLOVE
    POWERUP_LIFE,
    POWERUP_AMMO,    // Añade este si lo necesitas
    POWERUP_COUNT    // Mantén este como último
};

// Shop items
enum ShopItemType {
    SHOP_ITEM_LIFE = 0,
    SHOP_ITEM_BOOT,
    SHOP_ITEM_COFFEE,
    SHOP_ITEM_HAT,
    SHOP_ITEM_GLOVE
};

// Enemy types
enum EnemyType {
    ENEMY_ORC = 0,
    ENEMY_GHOST,
    ENEMY_OGRE,
    ENEMY_MUMMY,
    ENEMY_DEVIL,
    ENEMY_MUSHROOM,
    ENEMY_SPIKEY,
    ENEMY_BOSS_COWBOY,
    ENEMY_BOSS_FECTOR,
    ENEMY_MINI_BOSS,   // Mini-boss type
    ENEMY_BOSS         // Generic boss type
};

#define ENEMY_TYPES 9   // Total number of enemy types

// Enemy spawn types (for level data)
enum EnemySpawnType {
    ENEMY_SPAWN_ORC = 0,
    ENEMY_SPAWN_GHOST,
    ENEMY_SPAWN_OGRE,
    ENEMY_SPAWN_MUMMY,
    ENEMY_SPAWN_DEVIL,
    ENEMY_SPAWN_MUSHROOM,
    ENEMY_SPAWN_SPIKEY,
    ENEMY_SPAWN_BOSS_COWBOY,
    ENEMY_SPAWN_BOSS_FECTOR
};

// Tile types
enum TileType {
    TILE_FLOOR = 0,
    TILE_WALL,
    TILE_WATER,
    TILE_CACTUS,
    TILE_LAVA
};

// Cerca del final del archivo:

#endif // GAME_DEFS_H