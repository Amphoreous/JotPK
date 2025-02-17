#include "raylib.h"
#include "resource_dir.h"

typedef enum { MENU, GAME } GameScreen;

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Journey of the Prairie King");
    SetTargetFPS(60);
    SearchAndSetResourceDir("resources");
    Texture JotPKLogo = LoadTexture("JotPK_logo.png");

    GameScreen currentScreen = MENU;
    int selectedOption = 0;

    while (!WindowShouldClose())
    {
        if (currentScreen == MENU)
        {
            if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 3;
            if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + 3) % 3;
            if (IsKeyPressed(KEY_ENTER))
            {
                if (selectedOption == 0) currentScreen = GAME; // Iniciar juego
                else if (selectedOption == 1) {} // Configuración
                else if (selectedOption == 2) break; // Salir
            }

            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(JotPKLogo, (GetScreenWidth() - JotPKLogo.width) / 2, 50, WHITE);
            DrawText("Iniciar Juego", 190, 300, 20, selectedOption == 0 ? RED : WHITE);
            DrawText("Configuracion", 190, 350, 20, selectedOption == 1 ? RED : WHITE);
            DrawText("Salir", 190, 400, 20, selectedOption == 2 ? RED : WHITE);
            EndDrawing();
        }
        else if (currentScreen == GAME)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Juego en progreso...", 190, 200, 20, WHITE);
            EndDrawing();
        }
    }

    UnloadTexture(JotPKLogo);
    CloseWindow();
    return 0;
}
