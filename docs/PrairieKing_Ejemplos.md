# Ejemplos y Patrones de Uso

## Ejemplos de Código

### Creación de un Monstruo
```cpp
// Crear un orco en la posición (100, 100)
CowboyMonster* orc = new CowboyMonster(assets, ORC, Vector2{100, 100});

// Crear un fantasma con salud personalizada
CowboyMonster* ghost = new CowboyMonster(assets, GHOST, 2, GHOST_SPEED, Vector2{200, 200});
```

### Sistema de Disparo
```cpp
// Disparar en una dirección
void ShootInDirection(int direction) {
    Vector2 spawnPosition = m_playerPosition;
    std::vector<int> directions = {direction};
    SpawnBullets(directions, spawnPosition);
}

// Disparo en múltiples direcciones (power-up spread)
void SpreadShot() {
    Vector2 spawnPosition = m_playerPosition;
    std::vector<int> directions = {0, 1, 2, 3, 4, 5, 6, 7}; // 8 direcciones
    SpawnBullets(directions, spawnPosition);
}
```

### Sistema de Power-ups
```cpp
// Aplicar power-up de velocidad
void ApplySpeedPowerup() {
    m_speedBonus = 2; // Duplica la velocidad
    // Temporizador para la duración
    m_powerupTimers[POWERUP_SPEED] = POWERUP_DURATION;
}

// Verificar power-ups activos
void UpdatePowerups(float deltaTime) {
    for (auto& [type, timer] : m_powerupTimers) {
        if (timer > 0) {
            timer -= deltaTime;
            if (timer <= 0) {
                RemovePowerup(type);
            }
        }
    }
}
```

### Sistema de Oleadas
```cpp
// Iniciar una nueva oleada
void StartNewWave() {
    m_currentWave++;
    m_waveTimer = WAVE_DURATION;
    SpawnMonstersForWave();
    UpdateMonsterChancesForWave();
}

// Generar enemigos para la oleada
void SpawnMonstersForWave() {
    auto chances = GetMonsterChancesForWave(m_currentWave);
    for (int i = 0; i < m_monstersPerWave; i++) {
        int type = ChooseMonsterType(chances);
        Vector2 pos = GetRandomSpawnPosition();
        AddMonster(new CowboyMonster(assets, type, pos));
    }
}
```

## Patrones de Diseño

### Factory Pattern para Enemigos
```cpp
class MonsterFactory {
public:
    static CowboyMonster* CreateMonster(AssetManager& assets, int type, Vector2 position) {
        switch (type) {
            case ORC:
                return new CowboyMonster(assets, ORC, position);
            case GHOST:
                return new CowboyMonster(assets, GHOST, position);
            case OGRE:
                return new CowboyMonster(assets, OGRE, position);
            // ... otros tipos
            default:
                return nullptr;
        }
    }
};
```

### Observer Pattern para Eventos
```cpp
class GameEvent {
public:
    virtual void Notify() = 0;
};

class PlayerDeathEvent : public GameEvent {
    void Notify() override {
        // Lógica de muerte del jugador
        PlayerDie();
        AfterPlayerDeathFunction(0);
    }
};
```

### State Pattern para Enemigos Especiales
```cpp
class MonsterState {
public:
    virtual void Update(CowboyMonster* monster, float deltaTime) = 0;
    virtual void Enter(CowboyMonster* monster) = 0;
    virtual void Exit(CowboyMonster* monster) = 0;
};

class DraculaState : public MonsterState {
    void Update(CowboyMonster* monster, float deltaTime) override {
        // Lógica específica de Drácula
    }
    // ... otros métodos
};
```

## Ejemplos de Configuración

### Configuración de Mapa
```cpp
int map[MAP_WIDTH][MAP_HEIGHT] = {
    {MAP_BARRIER1, MAP_BARRIER1, MAP_BARRIER1, MAP_BARRIER1},
    {MAP_BARRIER1, MAP_DESERT,   MAP_DESERT,   MAP_BARRIER1},
    {MAP_BARRIER1, MAP_DESERT,   MAP_DESERT,   MAP_BARRIER1},
    {MAP_BARRIER1, MAP_BARRIER1, MAP_BARRIER1, MAP_BARRIER1}
};
```

### Configuración de Power-ups
```cpp
struct PowerupConfig {
    int duration;
    int effectValue;
    Color visualEffect;
};

std::map<int, PowerupConfig> powerupConfigs = {
    {POWERUP_SPEED, {10000, 2, YELLOW}},
    {POWERUP_RAPIDFIRE, {10000, 2, RED}},
    {POWERUP_LIFE, {0, 1, GREEN}} // Instantáneo
};
```

## Ejemplos de Debugging

### Visualización de Hitboxes
```cpp
void DrawDebugInfo() {
    // Dibujar hitbox del jugador
    DrawRectangleLinesEx(m_playerBoundingBox, 1, RED);
    
    // Dibujar hitboxes de enemigos
    for (auto monster : m_monsters) {
        DrawRectangleLinesEx(monster->position, 1, BLUE);
    }
    
    // Dibujar hitboxes de balas
    for (auto bullet : m_bullets) {
        DrawCircleV(bullet.position, 2, GREEN);
    }
}
```

### Registro de Eventos
```cpp
void LogGameEvent(const std::string& event) {
    std::cout << "[" << GetTime() << "] " << event << std::endl;
}

// Uso
LogGameEvent("Jugador recibió daño");
LogGameEvent("Power-up recolectado: " + std::to_string(powerupType));
LogGameEvent("Oleada completada: " + std::to_string(m_currentWave));
``` 