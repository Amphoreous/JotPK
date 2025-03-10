#include "raylib.h"
#include "game.h"
#include <cmath>
#include <cstdlib>

#define SHOOT_RATE 0.3f      // Tiempo en segundos entre disparos
#define ORC_SPAWN_RATE 2.0f  // Tiempo en segundos entre spawns de orcos

typedef struct Bullet {
    Rectangle frameRec_Bullet;
    Vector2 direction;
    bool active;
} Bullet;

typedef struct Orc {
    Rectangle frameRec_Orc;
    Vector2 direction;
    bool active;
} Orc;

void DrawGame(Texture2D Finn_Right, Texture2D Finn_Left, Texture2D Finn_Up, Texture2D Finn_Down,
    Texture2D Finn_Idle, Texture2D Finn_Shooting_Right, Texture2D Finn_Shooting_Left,
    Texture2D Finn_Shooting_Up, Texture2D Finn_Shooting_Down, Texture2D Bullet_1,
    Texture2D OrcTexture, Texture2D backgroundSpriteSheet, Music Overworld)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Variables para la animación del sprite sheet del fondo
    int spriteFrameCount = 18; // Número de frames en el sprite sheet
    int spriteFrameWidth = backgroundSpriteSheet.width / spriteFrameCount;
    int spriteFrameHeight = backgroundSpriteSheet.height;
    int currentSpriteFrame = 0;
    int spriteFrameSpeed = 8; // Velocidad de cambio de frame
    int spriteFrameCounter = 0;

    // Configuración de la animación del orco
    int OrcSpriteColumns = 4;
    int OrcFramesCounter = 0;
    int OrcFramesSpeed = 8;
    int OrcCurrentFrame = 0;

    // Calcular escala y dimensiones para el fondo
    float scale = fminf(static_cast<float>(screenWidth) / spriteFrameWidth, static_cast<float>(screenHeight) / spriteFrameHeight);
    float scaledWidth = spriteFrameWidth * scale;
    float scaledHeight = spriteFrameHeight * scale;
    Vector2 spritePosition = { (screenWidth - scaledWidth) / 2.0f, (screenHeight - scaledHeight) / 2.0f };

    // Calcular el tamaño del personaje usando scaledHeight / 16
    float characterSize = scaledHeight / 16.0f;

    // Tamaño de la bala
    float bulletSize = characterSize / 4.0f;

    // Posición inicial del jugador (centrado)
    Vector2 position = { screenWidth / 2.0f, screenHeight / 2.0f };

    // Setup de animación para las texturas (sprite sheets)
    int spriteColumns = 4;
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 8;

    // Rectángulos fuente para las texturas en movimiento
    Rectangle frameRec_Right = { 0.0f, 0.0f, static_cast<float>(Finn_Right.width) / spriteColumns, static_cast<float>(Finn_Right.height) };
    Rectangle frameRec_Left = { 0.0f, 0.0f, static_cast<float>(Finn_Left.width) / spriteColumns, static_cast<float>(Finn_Left.height) };
    Rectangle frameRec_Up = { 0.0f, 0.0f, static_cast<float>(Finn_Up.width) / spriteColumns, static_cast<float>(Finn_Up.height) };
    Rectangle frameRec_Down = { 0.0f, 0.0f, static_cast<float>(Finn_Down.width) / spriteColumns, static_cast<float>(Finn_Down.height) };
    Rectangle frameRec_Idle = { 0.0f, 0.0f, static_cast<float>(Finn_Idle.width), static_cast<float>(Finn_Idle.height) };
    Rectangle frameRec_Orc = { 0.0f, 0.0f, static_cast<float>(OrcTexture.width) / OrcSpriteColumns, static_cast<float>(OrcTexture.height) };

    // Textura y rectángulo fuente por defecto (idle)
    Texture2D currentTexture = Finn_Down;
    Rectangle* currentFrameRec = &frameRec_Down;
    Rectangle* OrcCurrentFrameRec = &frameRec_Orc;

    // Velocidad de movimiento base y factor de escala
    float baseMoveSpeed = 4.0f;
    float scaleFactor = screenHeight / 1080.0f; // Asumiendo 1080p como referencia
    float moveSpeed = baseMoveSpeed * scaleFactor;

    // Inicialización de balas y orcos
    Bullet* bullets = nullptr;
    int bulletCount = 0;
    Orc* orcs = nullptr;
    int orcCount = 0;

    float shootTimer = 0.0f;
    float orcSpawnTimer = 0.0f;

    // Margen para las balas (como tenías definido)
    float bulletMarginLeft = -15.0f;
    float bulletMarginRight = -0.08f;
    float bulletMarginTop = -5.0f;
    float bulletMarginBottom = -5.0f;

    // Dirección de disparo actual
    Vector2 bulletDirection = { 0.0f, 0.0f };

    // Game state variables
    int lives = 3;
    int coins = 0;
    int score = 0;
    int level = 1;
    bool gameOver = false;
    bool levelComplete = false;

    // Load UI elements
    Texture2D counterLives = LoadTexture("Counter_Lives.png");
    Texture2D counterCoins = LoadTexture("Counter_Coins.png");

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        shootTimer += deltaTime;
        orcSpawnTimer += deltaTime;
        float orcSize = characterSize - 10.0f;

        // Variables de movimiento del jugador
        bool isMoving = false;
        float moveX = 0.0f;
        float moveY = 0.0f;

        // Movement using WASD
        if (IsKeyDown(KEY_W)) moveY -= 1.0f;
        if (IsKeyDown(KEY_S)) moveY += 1.0f;
        if (IsKeyDown(KEY_A)) moveX -= 1.0f;
        if (IsKeyDown(KEY_D)) moveX += 1.0f;

        // Configurar dirección de disparo con arrow keys
        bulletDirection = { 0.0f, 0.0f };
        if (IsKeyDown(KEY_RIGHT)) bulletDirection.x += 1.0f;
        if (IsKeyDown(KEY_LEFT))  bulletDirection.x -= 1.0f;
        if (IsKeyDown(KEY_UP))    bulletDirection.y -= 1.0f;
        if (IsKeyDown(KEY_DOWN))  bulletDirection.y += 1.0f;

        // Disparo de balas
        if ((bulletDirection.x != 0.0f || bulletDirection.y != 0.0f) && shootTimer >= SHOOT_RATE)
        {
            float length = std::sqrt(bulletDirection.x * bulletDirection.x + bulletDirection.y * bulletDirection.y);
            bulletDirection.x /= length;
            bulletDirection.y /= length;

            bullets = static_cast<Bullet*>(std::realloc(bullets, (bulletCount + 1) * sizeof(Bullet)));
            bullets[bulletCount].frameRec_Bullet = { position.x, position.y, bulletSize, bulletSize };
            bullets[bulletCount].direction = bulletDirection;
            bullets[bulletCount].active = true;
            bulletCount++;
            shootTimer = 0.0f; // Reiniciar temporizador de disparo
        }

        // ---------------------------------------------------------------------
        // Spawn de orcos: el spawn se restringe a la zona central de 3/16 partes
        // de cada borde (dejando 6,5/16 partes en cada extremo sin spawn)
        if (orcSpawnTimer >= ORC_SPAWN_RATE)
        {
            // Dividir el borde en 16 partes
            float segmentWidth = scaledWidth / 16.0f;
            float segmentHeight = scaledHeight / 16.0f;
            // Zona permitida: 3 segmentos centrales
            float allowedWidth = 3.0f * segmentWidth;
            float allowedHeight = 3.0f * segmentHeight;
            // Offset para el inicio de la zona permitida (6.5 segmentos desde el borde)
            float offsetX = 6.5f * segmentWidth;
            float offsetY = 6.5f * segmentHeight;

            orcs = static_cast<Orc*>(std::realloc(orcs, (orcCount + 1) * sizeof(Orc)));
            orcs[orcCount].active = true;

            int spawnLocation = std::rand() % 4;
            switch (spawnLocation)
            {
            case 0: { // Top (parte superior)
                float randomX = spritePosition.x + offsetX +
                    static_cast<float>(std::rand() % (int)allowedWidth);
                orcs[orcCount].frameRec_Orc = { randomX, spritePosition.y, characterSize, characterSize };
                break;
            }
            case 1: { // Bottom (parte inferior)
                float randomX = spritePosition.x + offsetX +
                    static_cast<float>(std::rand() % (int)allowedWidth);
                orcs[orcCount].frameRec_Orc = { randomX, spritePosition.y + scaledHeight - characterSize, characterSize, characterSize };
                break;
            }
            case 2: { // Left (lado izquierdo)
                float randomY = spritePosition.y + offsetY +
                    static_cast<float>(std::rand() % (int)allowedHeight);
                orcs[orcCount].frameRec_Orc = { spritePosition.x, randomY, characterSize, characterSize };
                break;
            }
            case 3: { // Right (lado derecho)
                float randomY = spritePosition.y + offsetY +
                    static_cast<float>(std::rand() % (int)allowedHeight);
                orcs[orcCount].frameRec_Orc = { spritePosition.x + scaledWidth - characterSize, randomY, characterSize, characterSize };
                break;
            }
            }
            // La dirección se actualizará en cada frame
            orcs[orcCount].direction = { 0.0f, 0.0f };
            orcCount++;
            orcSpawnTimer = 0.0f; // Reiniciar temporizador de spawn de orcos
        }
        // ---------------------------------------------------------------------

        // Actualizar balas
        for (int i = 0; i < bulletCount; i++)
        {
            if (bullets[i].active)
            {
                bullets[i].frameRec_Bullet.x += bullets[i].direction.x * 10.0f;
                bullets[i].frameRec_Bullet.y += bullets[i].direction.y * 10.0f;

                if (bullets[i].frameRec_Bullet.x - bulletSize / 2.0f < spritePosition.x + bulletMarginLeft ||
                    bullets[i].frameRec_Bullet.x + bulletSize / 2.0f > spritePosition.x + scaledWidth - bulletMarginRight ||
                    bullets[i].frameRec_Bullet.y - bulletSize / 2.0f < spritePosition.y + bulletMarginTop ||
                    bullets[i].frameRec_Bullet.y + bulletSize / 2.0f > spritePosition.y + scaledHeight - bulletMarginBottom)
                {
                    bullets[i].active = false;
                }
            }
        }

        // Actualizar orcos: se calcula la dirección para que sigan al jugador moviéndose
        // únicamente en el eje con mayor diferencia (movimiento "en escalera")
        for (int i = 0; i < orcCount; i++)
        {
            if (orcs[i].active)
            {
                // Calcular el centro del orco
                float orcCenterX = orcs[i].frameRec_Orc.x + orcs[i].frameRec_Orc.width / 2.0f;
                float orcCenterY = orcs[i].frameRec_Orc.y + orcs[i].frameRec_Orc.height / 2.0f;

                // Diferencia respecto a la posición del jugador
                float diffX = position.x - orcCenterX;
                float diffY = position.y - orcCenterY;

                // Moverse en el eje con mayor diferencia
                if (std::abs(diffX) > std::abs(diffY))
                {
                    orcs[i].direction.x = (diffX > 0) ? 1.0f : -1.0f;
                    orcs[i].direction.y = 0.0f;
                }
                else
                {
                    orcs[i].direction.y = (diffY > 0) ? 1.0f : -1.0f;
                    orcs[i].direction.x = 0.0f;
                }

                // Actualizar posición del orco con velocidad fija
                orcs[i].frameRec_Orc.x += orcs[i].direction.x * 2.0f;
                orcs[i].frameRec_Orc.y += orcs[i].direction.y * 2.0f;

                // Verificar colisiones con balas
                for (int j = 0; j < bulletCount; j++)
                {
                    if (bullets[j].active && CheckCollisionRecs(orcs[i].frameRec_Orc, bullets[j].frameRec_Bullet))
                    {
                        orcs[i].active = false;
                        bullets[j].active = false;
                        
                        // Add coins and score when defeating an enemy
                        coins += 1;
                        score += 10;
                        
                        // Random chance to drop a powerup
                        if (GetRandomValue(0, 100) < 10) {
                            // TODO: Implement power-up system
                        }
                        
                        break;
                    }
                }

                // Verificar límites del fondo
                if (orcs[i].frameRec_Orc.x < spritePosition.x ||
                    orcs[i].frameRec_Orc.x + orcs[i].frameRec_Orc.width > spritePosition.x + scaledWidth ||
                    orcs[i].frameRec_Orc.y < spritePosition.y ||
                    orcs[i].frameRec_Orc.y + orcs[i].frameRec_Orc.height > spritePosition.y + scaledHeight)
                {
                    orcs[i].active = false;
                }
            }
        }

        // Add this in your game loop, after updating orcs but before drawing

        // Check for collisions between player and orcs
        for (int i = 0; i < orcCount; i++) {
            if (orcs[i].active) {
                // Create rectangles for collision detection
                Rectangle playerRect = { 
                    position.x - characterSize / 3.0f, 
                    position.y - characterSize / 3.0f, 
                    characterSize / 1.5f, 
                    characterSize / 1.5f 
                };
                
                Rectangle orcRect = { 
                    orcs[i].frameRec_Orc.x + orcSize / 4.0f, 
                    orcs[i].frameRec_Orc.y + orcSize / 4.0f, 
                    orcSize / 2.0f, 
                    orcSize / 2.0f 
                };
                
                if (CheckCollisionRecs(playerRect, orcRect)) {
                    lives--;
                    orcs[i].active = false;
                    
                    if (lives <= 0) {
                        gameOver = true;
                    }
                    
                    // Implement a short invincibility period here
                    break;
                }
            }
        }

        // Check for game restart
        if (gameOver && IsKeyPressed(KEY_R)) {
            // Reset game state
            lives = 3;
            coins = 0;
            score = 0;
            level = 1;
            gameOver = false;
            
            // Reset player position
            position = { screenWidth / 2.0f, screenHeight / 2.0f };
            
            // Clear all enemies and bullets
            bulletCount = 0;
            bullets = static_cast<Bullet*>(std::realloc(bullets, bulletCount * sizeof(Bullet)));
            orcCount = 0;
            orcs = static_cast<Orc*>(std::realloc(orcs, orcCount * sizeof(Orc)));
        }

        // Eliminar balas inactivas
        for (int i = 0; i < bulletCount; i++)
        {
            if (!bullets[i].active)
            {
                for (int j = i; j < bulletCount - 1; j++)
                {
                    bullets[j] = bullets[j + 1];
                }
                bulletCount--;
                bullets = static_cast<Bullet*>(std::realloc(bullets, bulletCount * sizeof(Bullet)));
                i--;
            }
        }

        // Eliminar orcos inactivos
        for (int i = 0; i < orcCount; i++)
        {
            if (!orcs[i].active)
            {
                for (int j = i; j < orcCount - 1; j++)
                {
                    orcs[j] = orcs[j + 1];
                }
                orcCount--;
                orcs = static_cast<Orc*>(std::realloc(orcs, orcCount * sizeof(Orc)));
                i--;
            }
        }

        // Actualizar música de fondo
        UpdateMusicStream(Overworld);

        // Movimiento del jugador
        if (moveX != 0 || moveY != 0)
        {
            isMoving = true;
            float length = std::sqrt(moveX * moveX + moveY * moveY);
            moveX = (moveX / length) * moveSpeed;
            moveY = (moveY / length) * moveSpeed;

            float newX = position.x + moveX;
            float newY = position.y + moveY;
            float margin = characterSize;

            if (newX - characterSize / 2.0f >= spritePosition.x + margin &&
                newX + characterSize / 2.0f <= spritePosition.x + scaledWidth - margin)
            {
                position.x = newX;
            }
            if (newY - characterSize / 2.0f >= spritePosition.y + margin &&
                newY + characterSize / 2.0f <= spritePosition.y + scaledHeight - margin)
            {
                position.y = newY;
            }
        }

        // Seleccionar textura según dirección de disparo o movimiento
        if (bulletDirection.x != 0.0f || bulletDirection.y != 0.0f)
        {
            if (bulletDirection.y < 0)
            {
                currentTexture = Finn_Up;
                currentFrameRec = &frameRec_Up;
            }
            if (bulletDirection.y > 0)
            {
                currentTexture = Finn_Down;
                currentFrameRec = &frameRec_Down;
            }
            if (bulletDirection.x > 0)
            {
                currentTexture = Finn_Right;
                currentFrameRec = &frameRec_Right;
            }
            if (bulletDirection.x < 0)
            {
                currentTexture = Finn_Left;
                currentFrameRec = &frameRec_Left;
            }
        }
        else if (isMoving)
        {
            if (moveY < 0)
            {
                currentTexture = Finn_Up;
                currentFrameRec = &frameRec_Up;
            }
            if (moveY > 0)
            {
                currentTexture = Finn_Down;
                currentFrameRec = &frameRec_Down;
            }
            if (moveX > 0)
            {
                currentTexture = Finn_Right;
                currentFrameRec = &frameRec_Right;
            }
            if (moveX < 0)
            {
                currentTexture = Finn_Left;
                currentFrameRec = &frameRec_Left;
            }
        }
        else
        {
            currentTexture = Finn_Idle;
            currentFrameRec = &frameRec_Idle;
        }

        // Actualizar animación del jugador
        if (isMoving)
        {
            framesCounter++;
            if (framesCounter >= (60 / framesSpeed))
            {
                framesCounter = 0;
                currentFrame = (currentFrame + 1) % spriteColumns;
                currentFrameRec->x = static_cast<float>(currentFrame) * currentFrameRec->width;
            }
        }

        // Actualizar animación del fondo
        spriteFrameCounter++;
        if (spriteFrameCounter >= (60 / spriteFrameSpeed))
        {
            spriteFrameCounter = 0;
            currentSpriteFrame = (currentSpriteFrame + 1) % spriteFrameCount;
        }

        // Actualizar animación del orco
        OrcFramesCounter++;
        if (OrcFramesCounter >= (60 / OrcFramesSpeed))
        {
            OrcFramesCounter = 0;
            OrcCurrentFrame = (OrcCurrentFrame + 1) % OrcSpriteColumns;
            OrcCurrentFrameRec->x = static_cast<float>(OrcCurrentFrame) * OrcCurrentFrameRec->width;
        }

        // Calcular el rectángulo fuente para el fondo
        int frameX = currentSpriteFrame * spriteFrameWidth;
        Rectangle spriteFrameRec = { static_cast<float>(frameX), 0.0f, static_cast<float>(spriteFrameWidth), static_cast<float>(spriteFrameHeight) };

        BeginDrawing();
        ClearBackground(BLACK);

        // Dibujar fondo
        DrawTexturePro(backgroundSpriteSheet, spriteFrameRec,
            { spritePosition.x, spritePosition.y, scaledWidth, scaledHeight },
            { 0, 0 }, 0.0f, WHITE);

        // Recalcular posición para dibujar al jugador centrado
        Vector2 drawPosition = { position.x - characterSize / 2.0f, position.y - characterSize / 2.0f };

        // Dibujar balas
        for (int i = 0; i < bulletCount; i++)
        {
            if (bullets[i].active)
            {
                DrawTexturePro(Bullet_1, { 0.0f, 0.0f, static_cast<float>(Bullet_1.width), static_cast<float>(Bullet_1.height) },
                    { bullets[i].frameRec_Bullet.x, bullets[i].frameRec_Bullet.y, bulletSize, bulletSize },
                    { 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }

        // Dibujar orcos
        for (int i = 0; i < orcCount; i++)
        {
            if (orcs[i].active)
            {
                DrawTexturePro(OrcTexture, frameRec_Orc,
                    { orcs[i].frameRec_Orc.x, orcs[i].frameRec_Orc.y, orcSize, orcSize },
                    { 0, 0 }, 0.0f, WHITE);
            }
        }

        // Dibujar jugador
        DrawTexturePro(currentTexture, *currentFrameRec,
            { drawPosition.x, drawPosition.y, characterSize, characterSize },
            { 0, 0 }, 0.0f, WHITE);

        // Draw UI elements
        float uiScale = characterSize / 32.0f; // Adjust scale based on character size
        int padding = 10;

        // Draw lives counter
        for (int i = 0; i < lives; i++) {
            DrawTexture(counterLives, padding + i * (counterLives.width + 5), padding, WHITE);
        }

        // Draw coins counter
        DrawTexture(counterCoins, padding, padding + counterLives.height + 5, WHITE);
        DrawText(TextFormat("%d", coins), padding + counterCoins.width + 5, 
                 padding + counterLives.height + 5 + counterCoins.height/4, 20, WHITE);

        // Draw score
        DrawText(TextFormat("SCORE: %d", score), screenWidth - 150, padding, 20, WHITE);

        // Draw level indicator
        DrawText(TextFormat("LEVEL: %d", level), screenWidth - 150, padding + 25, 20, WHITE);

        // Draw Game Over message if applicable
        if (gameOver) {
            const char* gameOverText = "GAME OVER - PRESS R TO RESTART";
            DrawText(gameOverText, 
                     screenWidth / 2 - MeasureText(gameOverText, 30) / 2,
                     screenHeight / 2, 
                     30, RED);
        }

        // Draw Level Complete message if applicable
        if (levelComplete) {
            const char* levelCompleteText = "LEVEL COMPLETE!";
            DrawText(levelCompleteText, 
                     screenWidth / 2 - MeasureText(levelCompleteText, 30) / 2,
                     screenHeight / 2, 
                     30, GREEN);
        }

        EndDrawing();
    }

    // Liberar memoria
    std::free(bullets);
    std::free(orcs);
}
