#include "raylib.h"
#include "game.h"

void DrawGame()
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Juego en progreso...", 190, 200, 20, WHITE);
    EndDrawing();
}