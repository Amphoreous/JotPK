#include "menu.h" // Include the header file with the function declarations

void UpdateMenu(GameScreen* currentScreen, int* selectedOption, int* hoveredOption)
{
    Vector2 mousePoint = GetMousePosition();
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int menuY = screenHeight - 100;
    int spacing = screenWidth / 5;

    // Resetear la opción sobre la que se pasa el cursor
    *hoveredOption = -1;

    // Entrada por teclado
    if (IsKeyPressed(KEY_DOWN))
    {
        *selectedOption = (*selectedOption + 1) % 4;
    }
    if (IsKeyPressed(KEY_UP))
    {
        *selectedOption = (*selectedOption - 1 + 4) % 4;
    }

    // Entrada por ratón
    if (CheckCollisionPointRec(mousePoint, (Rectangle) { spacing * 1 - MeasureText("Start", 20) / 2, menuY, MeasureText("Start", 20), 20 }))
    {
        *hoveredOption = 0;
        *selectedOption = -1; // Desactivar la opción del teclado
    }
    else if (CheckCollisionPointRec(mousePoint, (Rectangle) { spacing * 2 - MeasureText("Settings", 20) / 2, menuY, MeasureText("Settings", 20), 20 }))
    {
        *hoveredOption = 1;
        *selectedOption = -1; // Desactivar la opción del teclado
    }
    else if (CheckCollisionPointRec(mousePoint, (Rectangle) { spacing * 3 - MeasureText("How to play", 20) / 2, menuY, MeasureText("How to play", 20), 20 }))
    {
        *hoveredOption = 2;
        *selectedOption = -1; // Desactivar la opción del teclado
    }
    else if (CheckCollisionPointRec(mousePoint, (Rectangle) { spacing * 4 - MeasureText("Quit", 20) / 2, menuY, MeasureText("Quit", 20), 20 }))
    {
        *hoveredOption = 3;
        *selectedOption = -1; // Desactivar la opción del teclado
    }

    // Seleccionar opción
    if (IsKeyPressed(KEY_ENTER))
    {
        if (*selectedOption == 0) *currentScreen = GAME;     // Iniciar juego
        else if (*selectedOption == 1) *currentScreen = SETTINGS; // Configuración
        else if (*selectedOption == 2) *currentScreen = HTP;     // Cómo jugar
        else if (*selectedOption == 3) CloseWindow();           // Salir
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && *hoveredOption != -1)
    {
        if (*hoveredOption == 0) *currentScreen = GAME;     // Iniciar juego
        else if (*hoveredOption == 1) *currentScreen = SETTINGS; // Configuración
        else if (*hoveredOption == 2) *currentScreen = HTP;     // Cómo jugar
        else if (*hoveredOption == 3) CloseWindow();           // Salir
    }
}

void DrawMenu(Texture JotPKLogo, int selectedOption, int hoveredOption)
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

    // Colores por defecto
    Color startColor = WHITE;
    Color settingsColor = WHITE;
    Color htpColor = WHITE;
    Color quitColor = WHITE;

    // Determinar el color de cada opción
    if (hoveredOption >= 0)
    {
        // Si el ratón está sobre una opción
        if (hoveredOption == 0) startColor = RED;
        else if (hoveredOption == 1) settingsColor = RED;
        else if (hoveredOption == 2) htpColor = RED;
        else if (hoveredOption == 3) quitColor = RED;
    }
    else if (selectedOption >= 0)
    {
        // Si hay una opción seleccionada por teclado
        if (selectedOption == 0) startColor = RED;
        else if (selectedOption == 1) settingsColor = RED;
        else if (selectedOption == 2) htpColor = RED;
        else if (selectedOption == 3) quitColor = RED;
    }

    // Dibujar las opciones del menú horizontalmente
    DrawText("Start", spacing * 1 - MeasureText("Start", 20) / 2, menuY, 20, startColor);
    DrawText("Settings", spacing * 2 - MeasureText("Settings", 20) / 2, menuY, 20, settingsColor);
    DrawText("How to play", spacing * 3 - MeasureText("How to play", 20) / 2, menuY, 20, htpColor);
    DrawText("Quit", spacing * 4 - MeasureText("Quit", 20) / 2, menuY, 20, quitColor);

    EndDrawing();
}