#include "raylib.h"
#include "resource_dir.h"

typedef enum { MENU, GAME, SETTINGS, HTP } GameScreen;

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
        Vector2 mousePoint = GetMousePosition();

        if (currentScreen == MENU)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(JotPKLogo, (GetScreenWidth() - JotPKLogo.width) / 2, 50, WHITE);
            DrawText("Start", 190, 300, 20, selectedOption == 0 ? RED : WHITE);
            DrawText("Settings", 190, 350, 20, selectedOption == 1 ? RED : WHITE);
            DrawText("How to play", 190, 400, 20, selectedOption == 2 ? RED : WHITE);
            DrawText("Quit", 190, 450, 20, selectedOption == 3 ? RED : WHITE);
            EndDrawing();

            if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 4;
            if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + 4) % 4;

            if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 300, MeasureText("Start", 20), 20 })) selectedOption = 0;
            if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 350, MeasureText("Settings", 20), 20 })) selectedOption = 1;
            if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 400, MeasureText("How to play", 20), 20 })) selectedOption = 2;
            if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 450, MeasureText("Quit", 20), 20 })) selectedOption = 3;

            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (selectedOption == 0) currentScreen = GAME; // Iniciar juego
                else if (selectedOption == 1) currentScreen = SETTINGS; // Configuración
                else if (selectedOption == 2) currentScreen = HTP; // Cómo jugar
                else if (selectedOption == 3) break; // Salir
            }

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

            if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 500, MeasureText("Back", 20), 20 }) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
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

            if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 500, MeasureText("Back", 20), 20 }) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                currentScreen = MENU;
            }
        }

    }

    UnloadTexture(JotPKLogo);
    CloseWindow();
    return 0;
}
