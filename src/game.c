#include "raylib.h"
#include "game.h"




void DrawGame()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    Vector2 ballPosition = { (float)screenWidth / 2, (float)screenHeight / 2 }; //Modelo base, debe cambiarse


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_D)) ballPosition.x += 4.0f;
        if (IsKeyDown(KEY_A)) ballPosition.x -= 4.0f;
        if (IsKeyDown(KEY_W)) ballPosition.y -= 4.0f;
        if (IsKeyDown(KEY_S)) ballPosition.y += 4.0f;
        //----------------------------------------------------------------------------------

        // Draw
        //---------------------------------------------ç-------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        

        DrawCircleV(ballPosition, 50, MAROON);

        EndDrawing();
    }
    
}