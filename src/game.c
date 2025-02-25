#include "raylib.h"
#include "game.h"
#include <math.h> // Needed for sqrt()

void DrawGame()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // -------------------- Load textures -------------------- //

    // Main character
    Texture2D Finn_Right = LoadTexture("Sprite_Sheet_Right.png");
    Texture2D Finn_Left = LoadTexture("Sprite_Sheet_Left.png");
    Texture2D Finn_Up = LoadTexture("Sprite_Sheet_Up.png");
    Texture2D Finn_Down = LoadTexture("Sprite_Sheet_Down.png");
    Texture2D Finn_Idle = LoadTexture("PJ_Idle.png");

    // Enemies
    Texture2D Orc = LoadTexture("Sprite_Sheet_Orc.png");    // Orc

    // Load sprite sheet for background
    Texture2D backgroundSpriteSheet = LoadTexture("Sprite_Sheet_A1.png");

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
    float scale = fminf((float)screenWidth / spriteFrameWidth, (float)screenHeight / spriteFrameHeight);
    float scaledWidth = spriteFrameWidth * scale;
    float scaledHeight = spriteFrameHeight * scale;
    Vector2 spritePosition = { (screenWidth - scaledWidth) / 2.0f, (screenHeight - scaledHeight) / 2.0f };

    // Calcular el tamaño del personaje usando scaledHeight / 16
    float characterSize = scaledHeight / 16.0f;

    // Posición inicial del personaje en el centro de la pantalla
    Vector2 position = { screenWidth / 2.0f, screenHeight / 2.0f };

    // Setup de animación para las texturas con sprite sheets
    int spriteColumns = 4;
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 8;

    // Rectángulos fuente para las texturas en movimiento (sprite sheets)
    Rectangle frameRec_Right = { 0.0f, 0.0f, (float)Finn_Right.width / spriteColumns, (float)Finn_Right.height };
    Rectangle frameRec_Left = { 0.0f, 0.0f, (float)Finn_Left.width / spriteColumns, (float)Finn_Left.height };
    Rectangle frameRec_Up = { 0.0f, 0.0f, (float)Finn_Up.width / spriteColumns, (float)Finn_Up.height };
    Rectangle frameRec_Down = { 0.0f, 0.0f, (float)Finn_Down.width / spriteColumns, (float)Finn_Down.height };
    Rectangle frameRec_Idle = { 0.0f, 0.0f, (float)Finn_Idle.width, (float)Finn_Idle.height }; 
    Rectangle frameRec_Orc = { 0.0f, 0.0f, (float)Orc.width / OrcSpriteColumns, (float)Orc.height };

    // Textura y rectángulo fuente por defecto (idle)
    Texture2D currentTexture = Finn_Down;
    Rectangle* currentFrameRec = &frameRec_Down;

    Rectangle* OrcCurrentFrameRec = &frameRec_Orc;

    // Musica de fondo
    Music BackgroundMusic_A1 = LoadMusicStream("BackgroundMusic_A1.mp3");
    SetMusicVolume(BackgroundMusic_A1, 0.1f);
    PlayMusicStream(BackgroundMusic_A1);

    // Velocidad de movimiento
    float moveSpeed = 4.0f;

    while (!WindowShouldClose())
    {
        // Flags y variables de movimiento
        bool isMoving = false;
        float moveX = 0.0f;
        float moveY = 0.0f;

        if (IsKeyDown(KEY_D)) moveX += 1.0f;
        if (IsKeyDown(KEY_A)) moveX -= 1.0f;
        if (IsKeyDown(KEY_W)) moveY -= 1.0f;
        if (IsKeyDown(KEY_S)) moveY += 1.0f;

        // Bucle de música
        UpdateMusicStream(BackgroundMusic_A1);

        if (moveX != 0 || moveY != 0)
        {
            isMoving = true;
            // Normalización para el movimiento diagonal
            float length = sqrtf(moveX * moveX + moveY * moveY);
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

            // Seleccionar textura según la dirección
            if (moveY < 0) {
                currentTexture = Finn_Up;
                currentFrameRec = &frameRec_Up;
            }
            if (moveY > 0) {
                currentTexture = Finn_Down;
                currentFrameRec = &frameRec_Down;
            }
            if (moveX > 0) {
                currentTexture = Finn_Right;
                currentFrameRec = &frameRec_Right;
            }
            if (moveX < 0) {
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
                currentFrameRec->x = (float)currentFrame * currentFrameRec->width;
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
            OrcCurrentFrameRec->x = (float)OrcCurrentFrame * OrcCurrentFrameRec->width;
        }

        // Calcular el rectángulo fuente del frame actual del fondo
        int frameX = currentSpriteFrame * spriteFrameWidth;
        Rectangle spriteFrameRec = { (float)frameX, 0.0f, (float)spriteFrameWidth, (float)spriteFrameHeight };

        BeginDrawing();
        ClearBackground(BLACK);

        // Dibujar el fondo
        DrawTexturePro(backgroundSpriteSheet, spriteFrameRec, (Rectangle) { spritePosition.x, spritePosition.y, scaledWidth, scaledHeight }, (Vector2) { 0, 0 }, 0.0f, WHITE);

        // Calcular el tamaño del personaje usando scaledHeight / 16
        float characterSize = scaledHeight / 16.0f;
        Vector2 drawPosition = {
            position.x - characterSize / 2.0f,
            position.y - characterSize / 2.0f
        };

        float orcSize = scaledHeight / 16.0f - 10.0f;


        // Dibujar el personaje (en cualquier estado) con las medidas ajustadas
        DrawTexturePro(currentTexture, *currentFrameRec, (Rectangle) { drawPosition.x, drawPosition.y, characterSize, characterSize }, (Vector2) { 0, 0 }, 0.0f, WHITE);
        DrawTexturePro(Orc, frameRec_Orc, (Rectangle) { screenWidth/2, screenHeight/2, orcSize, orcSize
        }, (Vector2) { 0, 0 }, 0.0f, WHITE);

        EndDrawing();
    }

    // Liberar recursos
    UnloadTexture(Finn_Right);
    UnloadTexture(Finn_Left);
    UnloadTexture(Finn_Up);
    UnloadTexture(Finn_Down);
    UnloadTexture(Finn_Idle);
    UnloadTexture(Orc);
    UnloadTexture(backgroundSpriteSheet);
    UnloadMusicStream(BackgroundMusic_A1);
}