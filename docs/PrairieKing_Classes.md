# Documentación Detallada de Clases

## Clase PrairieKing

### Descripción
La clase principal del juego que maneja toda la lógica y estado del juego.

### Atributos Principales
- `m_assets`: Referencia al gestor de recursos
- `m_monsters`: Vector de punteros a enemigos
- `m_bullets`: Vector de balas del jugador
- `m_enemyBullets`: Vector de balas enemigas
- `m_powerups`: Vector de power-ups activos
- `m_temporarySprites`: Vector de sprites animados temporales
- `m_playerPosition`: Posición actual del jugador
- `m_map`: Matriz que representa el mapa actual

### Métodos Principales

#### Gestión del Juego
- `Initialize()`: Inicializa el estado del juego
- `Update(float deltaTime)`: Actualiza la lógica del juego
- `Draw()`: Renderiza el estado actual del juego
- `LoadGame()`: Carga el estado guardado
- `SaveGame()`: Guarda el estado actual
- `Reset()`: Reinicia el juego

#### Control del Jugador
- `UpdatePlayer(float deltaTime)`: Actualiza la lógica del jugador
- `AddPlayerMovementDirection(int direction)`: Añade una dirección de movimiento
- `AddPlayerShootingDirection(int direction)`: Añade una dirección de disparo
- `GetMovementSpeed(float speed, int directions)`: Calcula la velocidad de movimiento

#### Sistema de Combate
- `UpdateBullets(float deltaTime)`: Actualiza la lógica de las balas
- `SpawnBullets(const std::vector<int>& directions, Vector2 spawn)`: Crea nuevas balas
- `IsCollidingWithMonster(Rectangle r, CowboyMonster* subject)`: Detecta colisiones con monstruos

#### Sistema de Oleadas
- `StartNewWave()`: Inicia una nueva oleada
- `UpdateWaveTimer(float deltaTime)`: Actualiza el temporizador de oleada
- `HandleWaveCompletion()`: Maneja la finalización de una oleada
- `SpawnMonstersForWave()`: Genera enemigos para la oleada actual

## Clase CowboyMonster

### Descripción
Clase base para todos los tipos de enemigos en el juego.

### Atributos Principales
- `health`: Salud actual del monstruo
- `type`: Tipo de monstruo
- `speed`: Velocidad de movimiento
- `position`: Posición actual
- `movementDirection`: Dirección de movimiento actual

### Métodos Principales
- `Draw(const Texture2D& texture, Vector2 topLeftScreenCoordinate)`: Renderiza el monstruo
- `TakeDamage(int damage)`: Procesa el daño recibido
- `Move(Vector2 playerPosition, float deltaTime)`: Actualiza el movimiento
- `GetLootDrop()`: Determina el loot que suelta al morir

## Clase CowboyBullet

### Descripción
Maneja la lógica de las balas en el juego.

### Atributos Principales
- `position`: Posición actual de la bala
- `motion`: Vector de movimiento
- `damage`: Daño que causa la bala

### Métodos Principales
- Constructor con posición y dirección
- Constructor con posición y movimiento

## Clase CowboyPowerup

### Descripción
Gestiona los power-ups que pueden ser recogidos por el jugador.

### Atributos Principales
- `which`: Tipo de power-up
- `position`: Posición en el mapa
- `duration`: Duración del efecto
- `yOffset`: Desplazamiento vertical para animación

### Métodos Principales
- `Draw(const Texture2D& cursorTexture, Vector2 topLeftScreenCoordinate)`: Renderiza el power-up

## Clase TemporaryAnimatedSprite

### Descripción
Maneja sprites animados temporales para efectos visuales.

### Atributos Principales
- `sourceRect`: Rectángulo de origen en la textura
- `position`: Posición en pantalla
- `animationInterval`: Intervalo entre frames
- `frames`: Número total de frames
- `currentFrame`: Frame actual
- `rotation`: Rotación del sprite
- `scale`: Escala del sprite
- `tint`: Color del sprite

### Métodos Principales
- `Update(float deltaTime)`: Actualiza la animación
- `Draw(const Texture2D& texture)`: Renderiza el sprite

## Clases Especiales de Enemigos

### Dracula
Enemigo especial con múltiples fases de ataque:
- Fase de burla
- Fase de caminata y disparo
- Fase de disparo disperso
- Fase de invocación de demonios
- Fase de invocación de momias

### Outlaw
Enemigo especial con comportamiento de tirador:
- Fase de conversación
- Fase de escondite
- Fase de salto y disparo
- Fase de carrera y disparo
- Fase de disparo al jugador 