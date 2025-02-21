#include "raylib.h"
#include "game.h"
#include <math.h> // Needed for sqrt()

void DrawGame()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Load textures
    Texture2D Finn_Right = LoadTexture("Sprite_Sheet_Right.png");
    Texture2D Finn_Left = LoadTexture("Sprite_Sheet_Left.png");
    Texture2D Finn_Up = LoadTexture("Sprite_Sheet_Up.png");
    Texture2D Finn_Down = LoadTexture("Sprite_Sheet_Down.png");
    Texture2D Finn_Idle = LoadTexture("PJ_Idle.png");
    Texture2D gifTexture = LoadTexture("Area_1.gif");

    // Variables for animated gif
    int gifFrameWidth = gifTexture.width / 18; // Assuming 18 frames in the gif
    int gifFrameHeight = gifTexture.height;
    int gifFrameCount = 18; // Número de frames en el gif
    int currentGifFrame = 0;
    int gifFrameSpeed = 8; // Velocidad de cambio de frame (8 frames por segundo)
    int gifFrameCounter = 0;

    Vector2 position = { 350.0f, 280.0f }; // Character starting position

    // Animation setup
    int spriteColumns = 4;
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 8;

    // Define frame rectangles for each texture
    Rectangle frameRec_Right = { 0.0f, 0.0f, (float)Finn_Right.width / spriteColumns, (float)Finn_Right.height };
    Rectangle frameRec_Left = { 0.0f, 0.0f, (float)Finn_Left.width / spriteColumns, (float)Finn_Left.height };
    Rectangle frameRec_Up = { 0.0f, 0.0f, (float)Finn_Up.width / spriteColumns, (float)Finn_Up.height };
    Rectangle frameRec_Down = { 0.0f, 0.0f, (float)Finn_Down.width / spriteColumns, (float)Finn_Down.height };
    Rectangle frameRec_Idle = { 0.0f, 0.0f, (float)Finn_Idle.width, (float)Finn_Idle.height };

    // Default texture for idle
    Texture2D currentTexture = Finn_Idle;
    Rectangle* currentFrameRec = &frameRec_Idle;

    // Movement speed
    float moveSpeed = 4.0f;

    while (!WindowShouldClose())
    {
        // Movement flags
        bool isMoving = false;
        float moveX = 0.0f;
        float moveY = 0.0f;

        // Check movement separately for X and Y
        if (IsKeyDown(KEY_D)) { moveX += 1.0f; }
        if (IsKeyDown(KEY_A)) { moveX -= 1.0f; }
        if (IsKeyDown(KEY_W)) { moveY -= 1.0f; }
        if (IsKeyDown(KEY_S)) { moveY += 1.0f; }

        // Check if the player is moving diagonally
        if (moveX != 0 || moveY != 0)
        {
            isMoving = true;

            // Normalize diagonal movement to maintain speed
            float length = sqrtf(moveX * moveX + moveY * moveY);
            moveX = (moveX / length) * moveSpeed;
            moveY = (moveY / length) * moveSpeed;

            position.x += moveX;
            position.y += moveY;

            // Set texture based on direction (priority: last key pressed)
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

        // Update animation if moving
        if (isMoving)
        {
            framesCounter++;
            if (framesCounter >= (60 / framesSpeed))
            {
                framesCounter = 0;
                currentFrame = (currentFrame + 1) % spriteColumns;  // Loop through frames
                currentFrameRec->x = (float)currentFrame * currentFrameRec->width;
            }
        }
        else
        {
            // If not moving, keep idle texture
            currentTexture = Finn_Idle;
            currentFrameRec = &frameRec_Idle;
            currentFrameRec->x = 0.0f;  // Keeps the idle sprite at the first frame
        }

        // Update gif animation
        gifFrameCounter++;
        if (gifFrameCounter >= (60 / gifFrameSpeed))
        {
            gifFrameCounter = 0;
            currentGifFrame = (currentGifFrame + 1) % gifFrameCount;
        }

        // Adjust position for centering the texture (dynamic centering based on frame size)
        Vector2 drawPosition = {
            position.x - currentFrameRec->width / 2.0f,  // Center the texture horizontally
            position.y - currentFrameRec->height / 2.0f  // Center the texture vertically
        };

        // Draw
        BeginDrawing();
        ClearBackground(BLACK); // Cambia el fondo a negro para los bordes

        // Draw the current frame of the gif centered and scaled to fit the screen
        Rectangle gifFrameRec = { (float)(currentGifFrame * gifFrameWidth), 0.0f, (float)gifFrameWidth, (float)gifFrameHeight };
        float scale = fminf((float)screenWidth / gifFrameWidth, (float)screenHeight / gifFrameHeight);
        float scaledWidth = gifFrameWidth * scale;
        float scaledHeight = gifFrameHeight * scale;
        Vector2 gifPosition = { (screenWidth - scaledWidth) / 2.0f, (screenHeight - scaledHeight) / 2.0f };
        DrawTexturePro(gifTexture, gifFrameRec, (Rectangle) { gifPosition.x, gifPosition.y, scaledWidth, scaledHeight }, (Vector2) { 0, 0 }, 0.0f, WHITE);

        DrawTextureRec(currentTexture, *currentFrameRec, drawPosition, WHITE);
        EndDrawing();
    }

    // Cleanup
    UnloadTexture(Finn_Right);
    UnloadTexture(Finn_Left);
    UnloadTexture(Finn_Up);
    UnloadTexture(Finn_Down);
    UnloadTexture(Finn_Idle);
    UnloadTexture(gifTexture);
}
