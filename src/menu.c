#include "menu.h"
#include <math.h>  // Necesario para fabs()

// Variable estática interna para almacenar la opción "hovered" (sobre la que está el cursor)
static int hoveredOption = -1;

void DrawMenu(Texture JotPKLogo, int selectedOption)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Calcular el tamaño del logo basado en las dimensiones de la pantalla
    int logoWidth = screenWidth / 2;
    int logoHeight = (JotPKLogo.height * logoWidth) / JotPKLogo.width;

    // Calcular la posición de las opciones del menú
    int menuY = screenHeight - 100;
    int spacing = screenWidth / 5;

    // Calcular la posición del logo para que esté centrado en el espacio entre
    // el borde superior y el inicio de las opciones del menú.
    int logoX = (screenWidth - logoWidth) / 2;
    int logoY = (menuY / 2) - (logoHeight / 2);

    BeginDrawing();
    ClearBackground(BLACK);

    // Dibujar el logo centrado en el área disponible
    DrawTextureEx(JotPKLogo, (Vector2) { logoX, logoY }, 0.0f, (float)logoWidth / JotPKLogo.width, WHITE);

    // Colores por defecto para cada opción
    Color startColor = WHITE;
    Color settingsColor = WHITE;
    Color htpColor = WHITE;
    Color quitColor = WHITE;

    // Se resalta en rojo la opción correspondiente:
    // Si se está en modo ratón (hoveredOption != -1) se resalta esa opción.
    // En caso contrario, si hay una selección por teclado, se resalta esa.
    if (hoveredOption >= 0)
    {
        if (hoveredOption == 0) startColor = RED;
        else if (hoveredOption == 1) settingsColor = RED;
        else if (hoveredOption == 2) htpColor = RED;
        else if (hoveredOption == 3) quitColor = RED;
    }
    else if (selectedOption >= 0)
    {
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

void UpdateMenu(GameScreen* currentScreen, int* selectedOption)
{
    // Variable estática para almacenar la posición del ratón del frame anterior
    static Vector2 previousMousePosition = { 0, 0 };
    // Flag para indicar si se está en "modo ratón" o "modo teclado"
    static bool usingMouse = false;

    Vector2 mousePoint = GetMousePosition();
    // Umbral para evitar que pequeñas fluctuaciones se consideren movimiento
    float threshold = 0.1f;
    bool currentMouseMoved = (fabs(mousePoint.x - previousMousePosition.x) > threshold ||
        fabs(mousePoint.y - previousMousePosition.y) > threshold);
    previousMousePosition = mousePoint;

    // Si se pulsa alguna tecla de navegación, se activa el modo teclado
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT))
    {
        usingMouse = false;
    }
    // Si se detecta movimiento significativo del ratón, se activa el modo ratón
    if (currentMouseMoved)
    {
        usingMouse = true;
    }

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int menuY = screenHeight - 100;
    int spacing = screenWidth / 5;

    if (usingMouse)
    {
        // MODO RATÓN:
        // Se limpia la selección por teclado
        *selectedOption = -1;
        // Detectar colisión con cada opción para asignar hoveredOption
        if (CheckCollisionPointRec(mousePoint,
            (Rectangle) {
            spacing * 1 - MeasureText("Start", 20) / 2, menuY, MeasureText("Start", 20), 20
        }))
        {
            hoveredOption = 0;
        }
        else if (CheckCollisionPointRec(mousePoint,
            (Rectangle) {
            spacing * 2 - MeasureText("Settings", 20) / 2, menuY, MeasureText("Settings", 20), 20
        }))
        {
            hoveredOption = 1;
        }
        else if (CheckCollisionPointRec(mousePoint,
            (Rectangle) {
            spacing * 3 - MeasureText("How to play", 20) / 2, menuY, MeasureText("How to play", 20), 20
        }))
        {
            hoveredOption = 2;
        }
        else if (CheckCollisionPointRec(mousePoint,
            (Rectangle) {
            spacing * 4 - MeasureText("Quit", 20) / 2, menuY, MeasureText("Quit", 20), 20
        }))
        {
            hoveredOption = 3;
        }
        else
        {
            // Si el ratón se mueve fuera de las áreas de las opciones, se limpian ambos indicadores
            hoveredOption = -1;
            *selectedOption = -1;
        }
    }
    else
    {
        // MODO TECLADO:
        hoveredOption = -1;
        if (IsKeyPressed(KEY_RIGHT))
        {
            if (*selectedOption < 0)
                *selectedOption = 0;
            else
                *selectedOption = (*selectedOption + 1) % 4;
        }
        if (IsKeyPressed(KEY_LEFT))
        {
            if (*selectedOption < 0)
                *selectedOption = 0;
            else
                *selectedOption = (*selectedOption - 1 + 4) % 4;
        }
    }

    // Ejecutar la acción de la opción seleccionada
    if (IsKeyPressed(KEY_ENTER))
    {
        if (*selectedOption == 0) *currentScreen = GAME;       // Iniciar juego
        else if (*selectedOption == 1) *currentScreen = SETTINGS; // Configuración
        else if (*selectedOption == 2) *currentScreen = HTP;      // Cómo jugar
        else if (*selectedOption == 3) CloseWindow();             // Salir
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoveredOption != -1)
    {
        if (hoveredOption == 0) *currentScreen = GAME;       // Iniciar juego
        else if (hoveredOption == 1) *currentScreen = SETTINGS; // Configuración
        else if (hoveredOption == 2) *currentScreen = HTP;      // Cómo jugar
        else if (hoveredOption == 3) CloseWindow();             // Salir
    }
}