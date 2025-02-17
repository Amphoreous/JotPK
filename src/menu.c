#include "menu.h"

void DrawMenu(Texture JotPKLogo, int selectedOption)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(JotPKLogo, (GetScreenWidth() - JotPKLogo.width) / 2, 50, WHITE);
    DrawText("Start", 190, 300, 20, selectedOption == 0 ? RED : WHITE);
    DrawText("Settings", 190, 350, 20, selectedOption == 1 ? RED : WHITE);
    DrawText("How to play", 190, 400, 20, selectedOption == 2 ? RED : WHITE);
    DrawText("Quit", 190, 450, 20, selectedOption == 3 ? RED : WHITE);
    EndDrawing();
}

void UpdateMenu(GameScreen* currentScreen, int* selectedOption)
{
    Vector2 mousePoint = GetMousePosition();

    if (IsKeyPressed(KEY_DOWN)) *selectedOption = (*selectedOption + 1) % 4;
    if (IsKeyPressed(KEY_UP)) *selectedOption = (*selectedOption - 1 + 4) % 4;

    if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 300, MeasureText("Start", 20), 20 })) *selectedOption = 0;
    if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 350, MeasureText("Settings", 20), 20 })) *selectedOption = 1;
    if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 400, MeasureText("How to play", 20), 20 })) *selectedOption = 2;
    if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 450, MeasureText("Quit", 20), 20 })) *selectedOption = 3;

    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (*selectedOption == 0) *currentScreen = GAME; // Iniciar juego
        else if (*selectedOption == 1) *currentScreen = SETTINGS; // Configuración
        else if (*selectedOption == 2) *currentScreen = HTP; // Cómo jugar
        else if (*selectedOption == 3) CloseWindow(); // Salir
    }
}
