#include "menu.h"
#include <math.h>  // Necesario para fabs()

// Variable est�tica interna para almacenar la opci�n "hovered" (sobre la que est� el cursor)
static int hoveredOption = -1;

void DrawMenu(Texture JotPKLogo, int selectedOption)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Calcular el tama�o del logo basado en las dimensiones de la pantalla
    int logoWidth = screenWidth / 2;
    int logoHeight = (JotPKLogo.height * logoWidth) / JotPKLogo.width;

    // Calcular la posici�n de las opciones del men�
    int menuY = screenHeight - 100;
    int spacing = screenWidth / 5;

    // Calcular la posici�n del logo para que est� centrado en el espacio entre
    // el borde superior y el inicio de las opciones del men�.
    int logoX = (screenWidth - logoWidth) / 2;
    int logoY = (menuY / 2) - (logoHeight / 2);

    BeginDrawing();
    ClearBackground(BLACK);

    // Dibujar el logo centrado en el �rea disponible
    DrawTextureEx(JotPKLogo, (Vector2) { logoX, logoY }, 0.0f, (float)logoWidth / JotPKLogo.width, WHITE);

    // Colores por defecto para cada opci�n
    Color startColor = WHITE;
    Color settingsColor = WHITE;
    Color htpColor = WHITE;
    Color quitColor = WHITE;

    // Se resalta en rojo la opci�n correspondiente:
    // Si se est� en modo rat�n (hoveredOption != -1) se resalta esa opci�n.
    // En caso contrario, si hay una selecci�n por teclado, se resalta esa.
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

    // Dibujar las opciones del men� horizontalmente
    DrawText("Start", spacing * 1 - MeasureText("Start", 20) / 2, menuY, 20, startColor);
    DrawText("Settings", spacing * 2 - MeasureText("Settings", 20) / 2, menuY, 20, settingsColor);
    DrawText("How to play", spacing * 3 - MeasureText("How to play", 20) / 2, menuY, 20, htpColor);
    DrawText("Quit", spacing * 4 - MeasureText("Quit", 20) / 2, menuY, 20, quitColor);

    EndDrawing();
}

void UpdateMenu(GameScreen* currentScreen, int* selectedOption)
{
    // Variable est�tica para almacenar la posici�n del rat�n del frame anterior
    static Vector2 previousMousePosition = { 0, 0 };
    // Flag para indicar si se est� en "modo rat�n" o "modo teclado"
    static bool usingMouse = false;

    Vector2 mousePoint = GetMousePosition();
    // Umbral para evitar que peque�as fluctuaciones se consideren movimiento
    float threshold = 0.1f;
    bool currentMouseMoved = (fabs(mousePoint.x - previousMousePosition.x) > threshold ||
        fabs(mousePoint.y - previousMousePosition.y) > threshold);
    previousMousePosition = mousePoint;

    // Si se pulsa alguna tecla de navegaci�n, se activa el modo teclado
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT))
    {
        usingMouse = false;
    }
    // Si se detecta movimiento significativo del rat�n, se activa el modo rat�n
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
        // MODO RAT�N:
        // Se limpia la selecci�n por teclado
        *selectedOption = -1;
        // Detectar colisi�n con cada opci�n para asignar hoveredOption
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
            // Si el rat�n se mueve fuera de las �reas de las opciones, se limpian ambos indicadores
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

    // Ejecutar la acci�n de la opci�n seleccionada
    if (IsKeyPressed(KEY_ENTER))
    {
        if (*selectedOption == 0) *currentScreen = GAME;       // Iniciar juego
        else if (*selectedOption == 1) *currentScreen = SETTINGS; // Configuraci�n
        else if (*selectedOption == 2) *currentScreen = HTP;      // C�mo jugar
        else if (*selectedOption == 3) CloseWindow();             // Salir
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoveredOption != -1)
    {
        if (hoveredOption == 0) *currentScreen = GAME;       // Iniciar juego
        else if (hoveredOption == 1) *currentScreen = SETTINGS; // Configuraci�n
        else if (hoveredOption == 2) *currentScreen = HTP;      // C�mo jugar
        else if (hoveredOption == 3) CloseWindow();             // Salir
    }
}