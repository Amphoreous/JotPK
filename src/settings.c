#include "raylib.h"
#include "settings.h"

void DrawSettings(GameScreen* currentScreen)
{
    BeginDrawing();
    ClearBackground(BLACK);
    int textWidth = MeasureText("SETTINGS", 50);
    DrawText("SETTINGS", (GetScreenWidth() - textWidth) / 2, 100, 50, WHITE);
    DrawText("Sasdasdadassdad", 190, 300, 20, WHITE);
    DrawText("Back", 190, 500, 20, WHITE);
    EndDrawing();

    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { 190, 500, MeasureText("Back", 20), 20 }) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *currentScreen = MENU;
    }
}