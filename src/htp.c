#include "raylib.h"
#include "htp.h"

// Variable est�tica interna para almacenar la opci�n "hovered" (sobre la que est� el cursor)
static int hoveredOptionHTP = -1;
// Variable est�tica para almacenar la opci�n seleccionada por teclado
static int selectedOptionHTP = -1;
// Variable est�tica para almacenar la posici�n del rat�n del frame anterior
static Vector2 previousMousePositionHTP = { 0, 0 };
// Flag para indicar si se est� en "modo rat�n" o "modo teclado"
static bool usingMouseHTP = false;

void DrawHTP(GameScreen* currentScreen)
{
    BeginDrawing();
    ClearBackground(BLACK);
    int textWidth = MeasureText("HOW TO PLAY", 50);
    DrawText("HOW TO PLAY", (GetScreenWidth() - textWidth) / 2, 100, 50, WHITE);
    DrawText("Sasdasdadassdad", 190, 300, 20, WHITE);

    // Colores por defecto para el bot�n "Back"
    Color backColor = WHITE;

    // Resaltar en rojo si el rat�n est� sobre el bot�n "Back" o si est� seleccionado por teclado
    if (hoveredOptionHTP == 0 || selectedOptionHTP == 0)
    {
        backColor = RED;
    }

    DrawText("Back", 190, 500, 20, backColor);
    EndDrawing();

    Vector2 mousePoint = GetMousePosition();
    // Umbral para evitar que peque�as fluctuaciones se consideren movimiento
    float threshold = 0.1f;
    bool currentMouseMoved = (fabs(mousePoint.x - previousMousePositionHTP.x) > threshold ||
        fabs(mousePoint.y - previousMousePositionHTP.y) > threshold);
    previousMousePositionHTP = mousePoint;

    // Si se pulsa alguna tecla de navegaci�n, se activa el modo teclado
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN))
    {
        usingMouseHTP = false;
    }
    // Si se detecta movimiento significativo del rat�n, se activa el modo rat�n
    if (currentMouseMoved)
    {
        usingMouseHTP = true;
    }

    if (usingMouseHTP)
    {
        // MODO RAT�N:
        // Se limpia la selecci�n por teclado
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