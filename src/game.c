#include "raylib.h"
#include "game.h"
#include <math.h> // Needed for sqrt()

void DrawGame()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    InitWindow(screenWidth, screenHeight, "Sprite Animation");

    // Load textures
    Texture2D Finn_Right = LoadTexture("Sprite_Sheet_Right.png");
    Texture2D Finn_Left = LoadTexture("Sprite_Sheet_Left.png");
    Texture2D Finn_Up = LoadTexture("Sprite_Sheet_Up.png");
    Texture2D Finn_Down = LoadTexture("Sprite_Sheet_Down.png");
    Texture2D Finn_Idle = LoadTexture("PJ_Idle.png");

    // Check if textures are loaded properly
    if (Finn_Idle.id == 0 || Finn_Right.id == 0 || Finn_Left.id == 0 || Finn_Up.id == 0 || Finn_Down.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load one or more textures!");
        return;
    }

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

        // Adjust position for centering the texture (dynamic centering based on frame size)
        Vector2 drawPosition = {
            position.x - currentFrameRec->width / 2.0f,  // Center the texture horizontally
            position.y - currentFrameRec->height / 2.0f  // Center the texture vertically
        };

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextureRec(currentTexture, *currentFrameRec, drawPosition, WHITE);
        EndDrawing();
    }

    // Cleanup
    UnloadTexture(Finn_Right);
    UnloadTexture(Finn_Left);
    UnloadTexture(Finn_Up);
    UnloadTexture(Finn_Down);
    UnloadTexture(Finn_Idle);

    CloseWindow();
}
