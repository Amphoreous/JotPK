#include "raylib.h"
#include "settings.h"

// Variable estática interna para almacenar la opción "hovered" (sobre la que está el cursor)
static int hoveredOptionSettings = -1;
// Variable estática para almacenar la opción seleccionada por teclado
static int selectedOptionSettings = -1;
// Variable estática para almacenar la posición del ratón del frame anterior
static Vector2 previousMousePositionSettings = { 0, 0 };
// Flag para indicar si se está en "modo ratón" o "modo teclado"
static bool usingMouseSettings = false;

void DrawSettings(GameScreen* currentScreen)
{
    BeginDrawing();
    ClearBackground(BLACK);
    int textWidth = MeasureText("SETTINGS", 50);
    DrawText("SETTINGS", (GetScreenWidth() - textWidth) / 2, 100, 50, WHITE);
    DrawText("Sasdasdadassdad", 190, 300, 20, WHITE);

    // Colores por defecto para el botón "Back"
    Color backColor = WHITE;

    // Resaltar en rojo si el ratón está sobre el botón "Back" o si está seleccionado por teclado
    if (hoveredOptionSettings == 0 || selectedOptionSettings == 0)
    {
        backColor = RED;
    }

    DrawText("Back", 190, 500, 20, backColor);
    EndDrawing();

    Vector2 mousePoint = GetMousePosition();
    // Umbral para evitar que pequeñas fluctuaciones se consideren movimiento
    float threshold = 0.1f;
    bool currentMouseMoved = (fabs(mousePoint.x - previousMousePositionSettings.x) > threshold ||
        fabs(mousePoint.y - previousMousePositionSettings.y) > threshold);
    previousMousePositionSettings = mousePoint;

    // Si se pulsa alguna tecla de navegación, se activa el modo teclado
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN))
    {
        usingMouseSettings = false;
    }
    // Si se detecta movimiento significativo del ratón, se activa el modo ratón
    if (currentMouseMoved)
    {
        usingMouseSettings = true;
    }

    if (usingMouseSettings)
    {
        // MODO RATÓN:
        // Se limpia la selección por teclado
        selectedOptionSettings = -1;
        if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 500, MeasureText("Back", 20), 20 }))
        {
            hoveredOptionSettings = 0;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                *currentScreen = MENU;
            }
        }
        else
        {
            hoveredOptionSettings = -1;
        }
    }
    else
    {
        // MODO TECLADO:
        hoveredOptionSettings = -1;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP))
        {
            selectedOptionSettings = 0;
        }

        if (IsKeyPressed(KEY_ENTER) && selectedOptionSettings == 0)
        {
            *currentScreen = MENU;
        }
    }
}