#include "raylib.h"
#include "resource_dir.h"
#include "menu.h"

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Journey of the Prairie King");
    SetTargetFPS(60);
    SearchAndSetResourceDir("resources");
    Texture JotPKLogo = LoadTexture("JotPK.png");

    GameScreen currentScreen = MENU;
    int selectedOption = 0;

    while (!WindowShouldClose())
    {
        if (currentScreen == MENU)
        {
            DrawMenu(JotPKLogo, selectedOption);
            UpdateMenu(&currentScreen, &selectedOption);
        }
        else if (currentScreen == GAME)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Juego en progreso...", 190, 200, 20, WHITE);
            EndDrawing();
        }
        else if (currentScreen == SETTINGS)
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
                currentScreen = MENU;
            }
        }
        else if (currentScreen == HTP)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            int textWidth = MeasureText("HOW TO PLAY", 50);
            DrawText("HOW TO PLAY", (GetScreenWidth() - textWidth) / 2, 100, 50, WHITE);
            DrawText("Sasdasdadassdad", 190, 300, 20, WHITE);
            DrawText("Back", 190, 500, 20, WHITE);
            EndDrawing();

            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { 190, 500, MeasureText("Back", 20), 20 }) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                currentScreen = MENU;
            }
        }
    }

    UnloadTexture(JotPKLogo);
    CloseWindow();
    return 0;
}
