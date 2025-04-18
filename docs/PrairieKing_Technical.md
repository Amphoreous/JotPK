# Documentación Técnica

## Estructura del Proyecto

### Directorios Principales
- `src/`: Código fuente
- `include/`: Archivos de cabecera
- `resources/`: Recursos del juego
- `docs/`: Documentación

### Dependencias
- Raylib: Motor gráfico y de audio
- STL: Contenedores y algoritmos estándar
- C++17: Características del lenguaje

## Implementación de Clases

### PrairieKing
```cpp
class PrairieKing {
    // Gestión de estado
    bool m_isGameOver;
    bool m_gameOver;
    bool m_quit;
    bool m_died;
    bool m_onStartMenu;
    bool m_shopping;
    
    // Estado del jugador
    int m_runSpeedLevel;
    int m_fireSpeedLevel;
    int m_ammoLevel;
    int m_whichRound;
    int m_bulletDamage;
    int m_speedBonus;
    int m_fireRateBonus;
    int m_lives;
    int m_coins;
    int m_score;
    
    // Contenedores
    std::vector<CowboyMonster*> m_monsters;
    std::vector<CowboyBullet> m_bullets;
    std::vector<CowboyPowerup> m_powerups;
    std::vector<TemporaryAnimatedSprite> m_temporarySprites;
};
```

### CowboyMonster
```cpp
class CowboyMonster {
    // Atributos base
    int health;
    int type;
    int speed;
    Rectangle position;
    int movementDirection;
    
    // Estado
    bool movedLastTurn;
    bool oppositeMotionGuy;
    bool invisible;
    bool special;
    bool uninterested;
    bool flyer;
    
    // Efectos visuales
    Color tint;
    Color flashColor;
    float flashColorTimer;
};
```

### CowboyBullet
```cpp
class CowboyBullet {
    Vector2 position;
    Vector2 motion;
    int damage;
    
    // Constructores
    CowboyBullet(Vector2 position, Vector2 motion, int damage);
    CowboyBullet(Vector2 position, int direction, int damage);
};
```

## Sistemas de Colisión

### Detección de Colisiones
```cpp
bool IsCollidingWithMonster(Rectangle r, CowboyMonster* subject);
bool IsCollidingWithMap(Rectangle positionToCheck);
bool IsCollidingWithMap(Vector2 position);
```

### Tipos de Colisión
1. **Monstruo-Monstruo**
   - Prevención de superposición
   - Comportamiento de empuje

2. **Bala-Monstruo**
   - Detección de impacto
   - Cálculo de daño
   - Efectos visuales

3. **Jugador-Mapa**
   - Movimiento restringido
   - Detección de tiles especiales

## Sistema de Animación

### TemporaryAnimatedSprite
```cpp
class TemporaryAnimatedSprite {
    Rectangle sourceRect;
    Vector2 position;
    float animationInterval;
    int frames;
    int currentFrame;
    float rotation;
    float scale;
    bool flipped;
    float layerDepth;
    Color tint;
};
```

### Tipos de Animación
1. **Sprites Temporales**
   - Duración limitada
   - Efectos visuales
   - Transiciones suaves

2. **Animaciones de Personajes**
   - Movimiento
   - Disparo
   - Muerte

3. **Efectos Especiales**
   - Explosiones
   - Power-ups
   - Transiciones

## Sistema de Input

### GameKeys
```cpp
enum class GameKeys {
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    ShootLeft,
    ShootRight,
    ShootUp,
    ShootDown,
    UsePowerup,
    SelectOption,
    Exit,
    MAX
};
```

### Gestión de Input
```cpp
bool IsKeyDown(GameKeys key);
bool IsKeyPressed(GameKeys key);
bool IsKeyReleased(GameKeys key);
void SetButtonState(GameKeys key, bool pressed);
```

## Sistema de Guardado

### Estructura de Datos
```cpp
class JOTPKProgress {
    int bulletDamage;
    int fireSpeedLevel;
    int ammoLevel;
    bool spreadPistol;
    int runSpeedLevel;
    int lives;
    int coins;
    int score;
    bool died;
    int whichRound;
    int whichWave;
    int heldItem;
    int world;
    int waveTimer;
    std::vector<Vector2> monsterChances;
};
```

### Funciones de Guardado
```cpp
bool LoadGame();
void SaveGame();
void Reset();
```

## Optimizaciones

### Gestión de Memoria
- Uso de smart pointers
- Pool de objetos para balas
- Reutilización de sprites

### Rendimiento
- Culling de objetos fuera de pantalla
- Optimización de colisiones
- Gestión eficiente de animaciones

## Debugging

### Herramientas
- Visualización de hitboxes
- Modo de depuración
- Registro de eventos
- Contadores de rendimiento

### Métricas
- FPS
- Objetos activos
- Memoria utilizada
- Tiempo de actualización 