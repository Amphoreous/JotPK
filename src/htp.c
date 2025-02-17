#include "raylib.h"
#include "htp.h"

// Variable estática interna para almacenar la opción "hovered" (sobre la que está el cursor)
static int hoveredOptionHTP = -1;
// Variable estática para almacenar la opción seleccionada por teclado
static int selectedOptionHTP = -1;
// Variable estática para almacenar la posición del ratón del frame anterior
static Vector2 previousMousePositionHTP = { 0, 0 };
// Flag para indicar si se está en "modo ratón" o "modo teclado"
static bool usingMouseHTP = false;

void DrawHTP(GameScreen* currentScreen)
{
    BeginDrawing();
    ClearBackground(BLACK);
    int textWidth = MeasureText("HOW TO PLAY", 50);
    DrawText("HOW TO PLAY", (GetScreenWidth() - textWidth) / 2, 100, 50, WHITE);
    DrawText("Sasdasdadassdad", 190, 300, 20, WHITE);

    // Colores por defecto para el botón "Back"
    Color backColor = WHITE;

    // Resaltar en rojo si el ratón está sobre el botón "Back" o si está seleccionado por teclado
    if (hoveredOptionHTP == 0 || selectedOptionHTP == 0)
    {
        backColor = RED;
    }

    DrawText("Back", 190, 500, 20, backColor);
    EndDrawing();

    Vector2 mousePoint = GetMousePosition();
    // Umbral para evitar que pequeñas fluctuaciones se consideren movimiento
    float threshold = 0.1f;
    bool currentMouseMoved = (fabs(mousePoint.x - previousMousePositionHTP.x) > threshold ||
        fabs(mousePoint.y - previousMousePositionHTP.y) > threshold);
    previousMousePositionHTP = mousePoint;

    // Si se pulsa alguna tecla de navegación, se activa el modo teclado
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN))
    {
        usingMouseHTP = false;
    }
    // Si se detecta movimiento significativo del ratón, se activa el modo ratón
    if (currentMouseMoved)
    {
        usingMouseHTP = true;
    }

    if (usingMouseHTP)
    {
        // MODO RATÓN:
        // Se limpia la selección por teclado
        selectedOptionHTP = -1;
        if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 500, MeasureText("Back", 20), 20 }))
        {
            hoveredOptionHTP = 0;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                *currentScreen = MENU;
            }
        }
        else
        {
            hoveredOptionHTP = -1;
        }
    }
    else
    {
        // MODO TECLADO:
        hoveredOptionHTP = -1;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP))
        {
            selectedOptionHTP = 0;
        }

        if (IsKeyPressed(KEY_ENTER) && selectedOptionHTP == 0)
        {
            *currentScreen = MENU;
        }
    }
}