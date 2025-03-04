#include "raylib.h"
#include "settings.h"
#include <cmath>  // For fabs()

// Variables estáticas internas para almacenar estados

static int hoveredOptionSettings = -1;            // Opción sobre la que está el cursor
static int selectedOptionSettings = -1;           // Opción seleccionada por teclado
static Vector2 previousMousePositionSettings = { 0, 0 };
static bool usingMouseSettings = false;           // Flag: true = modo ratón, false = modo teclado

void DrawSettings(GameScreen* currentScreen)
{
    BeginDrawing();
    ClearBackground(BLACK);

    // Título y contenido

    int titleWidth = MeasureText("SETTINGS", 50);
    DrawText("SETTINGS", (GetScreenWidth() - titleWidth) / 2, 100, 50, WHITE);
    DrawText("Sasdasdadassdad", 190, 300, 20, WHITE);

    // "Back" Button

    Color backColor = WHITE;

    // Resalta en rojo si el ratón está sobre "Back" o si está seleccionado por teclado

    if (hoveredOptionSettings == 0 || selectedOptionSettings == 0)
    {
        backColor = RED;
    }
    DrawText("Back", 190, 500, 20, backColor);

    EndDrawing();

    // --- Actualización de input ---

    Vector2 mousePoint = GetMousePosition();

    // Umbral para evitar que pequeñas fluctuaciones activen el modo ratón

    float threshold = 0.1f;
    bool currentMouseMoved = (std::fabs(mousePoint.x - previousMousePositionSettings.x) > threshold ||
        std::fabs(mousePoint.y - previousMousePositionSettings.y) > threshold);
    previousMousePositionSettings = mousePoint;

    // Si se presiona una tecla de navegación, se activa el modo teclado

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))
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
        // --- MODO RATÓN ---

        selectedOptionSettings = -1;  // Limpiar la selección por teclado

        Rectangle backButtonRect = { 190, 500, static_cast<float>(MeasureText("Back", 20)), 20 };
        if (CheckCollisionPointRec(mousePoint, backButtonRect))
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
        // --- MODO TECLADO ---

        hoveredOptionSettings = -1;

        // Si aún no hay opción seleccionada, se asigna la única opción (índice 0)

        if (selectedOptionSettings < 0)
        {
            selectedOptionSettings = 0;
        }
        if (IsKeyPressed(KEY_ENTER) && selectedOptionSettings == 0)
        {
            *currentScreen = MENU;
        }
    }
}
