# Constantes y Configuración

## Constantes del Juego

### Tamaños y Escalas
```cpp
constexpr int BASE_TILE_SIZE = 16;
constexpr int PIXEL_ZOOM = 3;
constexpr int MAP_WIDTH = 16;
constexpr int MAP_HEIGHT = 16;
```

### Velocidades
```cpp
constexpr int ORC_SPEED = 2;
constexpr int OGRE_SPEED = 1;
constexpr int GHOST_SPEED = 3;
constexpr int SPIKEY_SPEED = 3;
constexpr float PLAYER_SPEED = 3.0f;
constexpr int BULLET_SPEED = 8;
```

### Salud
```cpp
constexpr int ORC_HEALTH = 1;
constexpr int GHOST_HEALTH = 1;
constexpr int OGRE_HEALTH = 3;
constexpr int SPIKEY_HEALTH = 2;
```

### Temporizadores
```cpp
constexpr int WAVE_DURATION = 80000;        // 80 segundos por oleada
constexpr int BETWEEN_WAVE_DURATION = 5000; // 5 segundos entre oleadas
constexpr int START_MENU_DURATION = 1500;   // 1.5 segundos para menú inicial
constexpr int DEATH_DELAY = 3000;           // 3 segundos de retraso por muerte
constexpr int PLAYER_INVINCIBLE_DURATION = 5000; // 5 segundos de invencibilidad
```

## Tipos de Monstruos
```cpp
constexpr int ORC = 0;
constexpr int GHOST = 1;
constexpr int OGRE = 2;
constexpr int MUMMY = 3;
constexpr int DEVIL = 4;
constexpr int MUSHROOM = 5;
constexpr int SPIKEY = 6;
constexpr int DRACULA = 7;
```

## Tipos de Power-ups
```cpp
constexpr int POWERUP_LOG = -1;
constexpr int POWERUP_SKULL = -2;
constexpr int POWERUP_HEART = -3;
constexpr int COIN1 = 0;
constexpr int COIN5 = 1;
constexpr int POWERUP_SPREAD = 2;
constexpr int POWERUP_RAPIDFIRE = 3;
constexpr int POWERUP_NUKE = 4;
constexpr int POWERUP_ZOMBIE = 5;
constexpr int POWERUP_SPEED = 6;
constexpr int POWERUP_SHOTGUN = 7;
constexpr int POWERUP_LIFE = 8;
constexpr int POWERUP_TELEPORT = 9;
constexpr int POWERUP_SHERRIFF = 10;
```

## Tipos de Items de Tienda
```cpp
constexpr int ITEM_FIRESPEED1 = 0;
constexpr int ITEM_FIRESPEED2 = 1;
constexpr int ITEM_FIRESPEED3 = 2;
constexpr int ITEM_RUNSPEED1 = 3;
constexpr int ITEM_RUNSPEED2 = 4;
constexpr int ITEM_LIFE = 5;
constexpr int ITEM_AMMO1 = 6;
constexpr int ITEM_AMMO2 = 7;
constexpr int ITEM_AMMO3 = 8;
constexpr int ITEM_SPREADPISTOL = 9;
constexpr int ITEM_STAR = 10;
constexpr int ITEM_SKULL = 11;
constexpr int ITEM_LOG = 12;
```

## Tipos de Tiles del Mapa
```cpp
constexpr int MAP_BARRIER1 = 0;
constexpr int MAP_BARRIER2 = 1;
constexpr int MAP_ROCKY1 = 2;
constexpr int MAP_DESERT = 3;
constexpr int MAP_GRASSY = 4;
constexpr int MAP_CACTUS = 5;
constexpr int MAP_FENCE = 7;
constexpr int MAP_TRENCH1 = 8;
constexpr int MAP_TRENCH2 = 9;
constexpr int MAP_BRIDGE = 10;
```

## Tipos de Mundo
```cpp
constexpr int DESERT_WORLD = 0;
constexpr int GRAVEYARD = 1;
constexpr int WOODS = 2;
```

## Opciones de Game Over
```cpp
constexpr int OPTION_RETRY = 0;
constexpr int OPTION_QUIT = 1;
```

## Probabilidades y Ratios
```cpp
constexpr double LOOT_CHANCE = 0.05;    // 5% de probabilidad de loot
constexpr double COIN_CHANCE = 0.05;    // 5% de probabilidad de moneda
```

## Duración de Efectos
```cpp
constexpr int LOOT_DURATION = 7500;     // 7.5 segundos para loot
constexpr int POWERUP_DURATION = 10000; // 10 segundos para power-ups
constexpr int CACTUS_DANCE_DELAY = 800; // 0.8 segundos para animación de cactus
constexpr int PLAYER_MOTION_DELAY = 100; // 0.1 segundos para movimiento del jugador
constexpr int PLAYER_FOOTSTEP_DELAY = 200; // 0.2 segundos para sonido de pasos
```

## Configuración de Rendimiento
```cpp
constexpr int MONSTER_ANIMATION_DELAY = 500; // 0.5 segundos para animación de monstruos
```

## Configuración de Juego
```cpp
constexpr int INITIAL_LIVES = 3;
constexpr int INITIAL_COINS = 0;
constexpr int INITIAL_SCORE = 0;
constexpr int INITIAL_WAVE = 1;
constexpr int INITIAL_ROUND = 1;
```

## Precios de Items
```cpp
constexpr int PRICE_FIRESPEED1 = 100;
constexpr int PRICE_FIRESPEED2 = 250;
constexpr int PRICE_FIRESPEED3 = 500;
constexpr int PRICE_RUNSPEED1 = 100;
constexpr int PRICE_RUNSPEED2 = 250;
constexpr int PRICE_LIFE = 1000;
constexpr int PRICE_AMMO1 = 100;
constexpr int PRICE_AMMO2 = 250;
constexpr int PRICE_AMMO3 = 500;
constexpr int PRICE_SPREADPISTOL = 1000;
constexpr int PRICE_STAR = 500;
constexpr int PRICE_SKULL = 750;
constexpr int PRICE_LOG = 750;
``` 