#include "menu.h"

void DrawMenu(Texture JotPKLogo, int selectedOption)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Calcular el tamaño del logo basado en las dimensiones de la pantalla
    int logoWidth = screenWidth / 2;
    int logoHeight = (JotPKLogo.height * logoWidth) / JotPKLogo.width;

    // Calcular la posición del logo para centrarlo
    int logoX = (screenWidth - logoWidth) / 2;
    int logoY = screenHeight / 10;

    // Calcular la posición de las opciones del menú
    int menuY = screenHeight - 100;
    int spacing = screenWidth / 5;

    BeginDrawing();
    ClearBackground(BLACK);

    // Dibujar el logo centrado
    DrawTextureEx(JotPKLogo, (Vector2) { logoX, logoY }, 0.0f, (float)logoWidth / JotPKLogo.width, WHITE);

    // Dibujar las opciones del menú horizontalmente
    DrawText("Start", spacing * 1 - MeasureText("Start", 20) / 2, menuY, 20, selectedOption == 0 ? RED : WHITE);
    DrawText("Settings", spacing * 2 - MeasureText("Settings", 20) / 2, menuY, 20, selectedOption == 1 ? RED : WHITE);
    DrawText("How to play", spacing * 3 - MeasureText("How to play", 20) / 2, menuY, 20, selectedOption == 2 ? RED : WHITE);
    DrawText("Quit", spacing * 4 - MeasureText("Quit", 20) / 2, menuY, 20, selectedOption == 3 ? RED : WHITE);

    EndDrawing();
}

void UpdateMenu(GameScreen* currentScreen, int* selectedOption)
{
    Vector2 mousePoint = GetMousePosition();
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int menuY = screenHeight - 100;
    int spacing = screenWidth / 5;

    if (IsKeyPressed(KEY_DOWN)) *selectedOption = (*selectedOption + 1) % 4;
    if (IsKeyPressed(KEY_UP)) *selectedOption = (*selectedOption - 1 + 4) % 4;

    if (CheckCollisionPointRec(mousePoint, (Rectangle) { spacing * 1 - MeasureText("Start", 20) / 2, menuY, MeasureText("Start", 20), 20 })) *selectedOption = 0;
    if (CheckCollisionPointRec(mousePoint, (Rectangle) { spacing * 2 - MeasureText("Settings", 20) / 2, menuY, MeasureText("Settings", 20), 20 })) *selectedOption = 1;
    if (CheckCollisionPointRec(mousePoint, (Rectangle) { spacing * 3 - MeasureText("How to play", 20) / 2, menuY, MeasureText("How to play", 20), 20 })) *selectedOption = 2;
    if (CheckCollisionPointRec(mousePoint, (Rectangle) { spacing * 4 - MeasureText("Quit", 20) / 2, menuY, MeasureText("Quit", 20), 20 })) *selectedOption = 3;

    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (*selectedOption == 0) *currentScreen = GAME; // Iniciar juego
        else if (*selectedOption == 1) *currentScreen = SETTINGS; // Configuración
        else if (*selectedOption == 2) *currentScreen = HTP; // Cómo jugar
        else if (*selectedOption == 3) CloseWindow(); // Salir
    }
}
