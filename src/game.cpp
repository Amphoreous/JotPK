#include "raylib.h"
#include "game.h"
#include <cmath>
#include <cstdlib>

#define SHOOT_RATE 0.3f // Tiempo en segundos entre disparos

typedef struct Bullet
{
    Rectangle frameRec_Bullet;
    Vector2 direction;
    bool active;
} Bullet;

void DrawGame(Texture2D Finn_Right, Texture2D Finn_Left, Texture2D Finn_Up, Texture2D Finn_Down, Texture2D Finn_Idle, Texture2D Finn_Shooting_Right, Texture2D Finn_Shooting_Left, Texture2D Finn_Shooting_Up, Texture2D Finn_Shooting_Down, Texture2D Bullet_1, Texture2D Orc, Texture2D backgroundSpriteSheet, Music BackgroundMusic_A1)
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

    // Orc animation setup
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

    // Posición inicial del personaje en el centro de la pantalla
    Vector2 position = { screenWidth / 2.0f, screenHeight / 2.0f };

    // Setup de animación para las texturas con sprite sheets
    int spriteColumns = 4;
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 8;

    // Rectángulos fuente para las texturas en movimiento (sprite sheets)
    Rectangle frameRec_Right = { 0.0f, 0.0f, static_cast<float>(Finn_Right.width) / spriteColumns, static_cast<float>(Finn_Right.height) };
    Rectangle frameRec_Left = { 0.0f, 0.0f, static_cast<float>(Finn_Left.width) / spriteColumns, static_cast<float>(Finn_Left.height) };
    Rectangle frameRec_Up = { 0.0f, 0.0f, static_cast<float>(Finn_Up.width) / spriteColumns, static_cast<float>(Finn_Up.height) };
    Rectangle frameRec_Down = { 0.0f, 0.0f, static_cast<float>(Finn_Down.width) / spriteColumns, static_cast<float>(Finn_Down.height) };
    Rectangle frameRec_Idle = { 0.0f, 0.0f, static_cast<float>(Finn_Idle.width), static_cast<float>(Finn_Idle.height) };
    Rectangle frameRec_Orc = { 0.0f, 0.0f, static_cast<float>(Orc.width) / OrcSpriteColumns, static_cast<float>(Orc.height) };

    // Textura y rectángulo fuente por defecto (idle)
    Texture2D currentTexture = Finn_Down;
    Rectangle* currentFrameRec = &frameRec_Down;

    Rectangle* OrcCurrentFrameRec = &frameRec_Orc;

    // Velocidad de movimiento
    float moveSpeed = 4.0f;

    // Initialize bullets
    Bullet* bullets = nullptr;
    int bulletCount = 0;

    float shootTimer = 0.0f;

    // Margen para las balas
    float bulletMarginLeft = -15.0f;
    float bulletMarginRight = -.08f;
    float bulletMarginTop = -5.0f;
    float bulletMarginBottom = -5.0f;

    // Dirección de disparo actual
    Vector2 bulletDirection = { 0.0f, 0.0f };

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        shootTimer += deltaTime;

        // Flags y variables de movimiento
        bool isMoving = false;
        float moveX = 0.0f;
        float moveY = 0.0f;

        if (IsKeyDown(KEY_D)) moveX += 1.0f;
        if (IsKeyDown(KEY_A)) moveX -= 1.0f;
        if (IsKeyDown(KEY_W)) moveY -= 1.0f;
        if (IsKeyDown(KEY_S)) moveY += 1.0f;

        // Shoot bullets
        bulletDirection = { 0.0f, 0.0f };
        if (IsKeyDown(KEY_RIGHT)) bulletDirection.x += 1.0f;
        if (IsKeyDown(KEY_LEFT)) bulletDirection.x -= 1.0f;
        if (IsKeyDown(KEY_UP)) bulletDirection.y -= 1.0f;
        if (IsKeyDown(KEY_DOWN)) bulletDirection.y += 1.0f;

        if ((bulletDirection.x != 0.0f || bulletDirection.y != 0.0f) && shootTimer >= SHOOT_RATE)
        {
            // Normalizar la dirección del disparo
            float length = std::sqrt(bulletDirection.x * bulletDirection.x + bulletDirection.y * bulletDirection.y);
            bulletDirection.x /= length;
            bulletDirection.y /= length;

            bullets = static_cast<Bullet*>(std::realloc(bullets, (bulletCount + 1) * sizeof(Bullet)));
            bullets[bulletCount].frameRec_Bullet = { position.x, position.y, bulletSize, bulletSize };
            bullets[bulletCount].direction = bulletDirection;
            bullets[bulletCount].active = true;
            bulletCount++;
            shootTimer = 0.0f; // Reiniciar el temporizador de disparo
        }

        // Update bullets
        for (int i = 0; i < bulletCount; i++)
        {
            if (bullets[i].active)
            {
                bullets[i].frameRec_Bullet.x += bullets[i].direction.x * 10.0f;
                bullets[i].frameRec_Bullet.y += bullets[i].direction.y * 10.0f;

                // Verificar límites antes de actualizar la posición de la bala
                if (bullets[i].frameRec_Bullet.x - bulletSize / 2.0f < spritePosition.x + bulletMarginLeft ||
                    bullets[i].frameRec_Bullet.x + bulletSize / 2.0f > spritePosition.x + scaledWidth - bulletMarginRight ||
                    bullets[i].frameRec_Bullet.y - bulletSize / 2.0f < spritePosition.y + bulletMarginTop ||
                    bullets[i].frameRec_Bullet.y + bulletSize / 2.0f > spritePosition.y + scaledHeight - bulletMarginBottom)
                {
                    bullets[i].active = false;
                }
            }
        }

        // Remove inactive bullets
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

        // Bucle de música
        UpdateMusicStream(BackgroundMusic_A1);

        if (moveX != 0 || moveY != 0)
        {
            isMoving = true;
            // Normalización para el movimiento diagonal
            float length = std::sqrt(moveX * moveX + moveY * moveY);
            moveX = (moveX / length) * moveSpeed;
            moveY = (moveY / length) * moveSpeed;

            // Verificar límites antes de actualizar la posición
            float newX = position.x + moveX;
            float newY = position.y + moveY;

            // Definir el margen
            float margin = characterSize;

            if (newX - characterSize / 2.0f >= spritePosition.x + margin && newX + characterSize / 2.0f <= spritePosition.x + scaledWidth - margin)
            {
                position.x = newX;
            }
            if (newY - characterSize / 2.0f >= spritePosition.y + margin && newY + characterSize / 2.0f <= spritePosition.y + scaledHeight - margin)
            {
                position.y = newY;
            }
        }

        // Seleccionar textura según la dirección de disparo
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
            // Seleccionar textura según la dirección de movimiento
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
            // Estado idle: usar la textura Finn_Idle
            currentTexture = Finn_Idle;
            currentFrameRec = &frameRec_Idle;
        }

        // Actualizar animación de los sprites en movimiento
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

        // Actualizar animación del sprite sheet de fondo
        spriteFrameCounter++;
        if (spriteFrameCounter >= (60 / spriteFrameSpeed))
        {
            spriteFrameCounter = 0;
            currentSpriteFrame = (currentSpriteFrame + 1) % spriteFrameCount;
        }

        // Orc animation frame update
        OrcFramesCounter++;
        if (OrcFramesCounter >= (60 / OrcFramesSpeed))
        {
            OrcFramesCounter = 0;
            OrcCurrentFrame = (OrcCurrentFrame + 1) % OrcSpriteColumns;
            OrcCurrentFrameRec->x = static_cast<float>(OrcCurrentFrame) * OrcCurrentFrameRec->width;
        }

        // Calcular el rectángulo fuente del frame actual del fondo
        int frameX = currentSpriteFrame * spriteFrameWidth;
        Rectangle spriteFrameRec = { static_cast<float>(frameX), 0.0f, static_cast<float>(spriteFrameWidth), static_cast<float>(spriteFrameHeight) };

        BeginDrawing();
        ClearBackground(BLACK);

        // Dibujar el fondo
        DrawTexturePro(backgroundSpriteSheet, spriteFrameRec, { spritePosition.x, spritePosition.y, scaledWidth, scaledHeight }, { 0, 0 }, 0.0f, WHITE);

        // Calcular el tamaño del personaje usando scaledHeight / 16
        float characterSize = scaledHeight / 16.0f;
        Vector2 drawPosition =
        {
            position.x - characterSize / 2.0f,
            position.y - characterSize / 2.0f
        };

        float orcSize = scaledHeight / 16.0f - 10.0f;

        // Draw bullets
        for (int i = 0; i < bulletCount; i++)
        {
            if (bullets[i].active)
            {
                DrawTexturePro(Bullet_1, { 0.0f, 0.0f, static_cast<float>(Bullet_1.width), static_cast<float>(Bullet_1.height) },
                    { bullets[i].frameRec_Bullet.x, bullets[i].frameRec_Bullet.y, bulletSize, bulletSize },
                    { 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }

        // Dibujar el personaje (en cualquier estado) con las medidas ajustadas
        DrawTexturePro(currentTexture, *currentFrameRec, { drawPosition.x, drawPosition.y, characterSize, characterSize }, { 0, 0 }, 0.0f, WHITE);
        DrawTexturePro(Orc, frameRec_Orc, { static_cast<float>(screenWidth) / 2.0f, static_cast<float>(screenHeight) / 2.0f, orcSize, orcSize }, { 0, 0 }, 0.0f, WHITE);

        EndDrawing();
    }

    // Liberar memoria de las balas
    std::free(bullets);
}
