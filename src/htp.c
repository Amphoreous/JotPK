#include "raylib.h"
#include "htp.h"
#include <math.h>  // Para fabs()

// Variables estáticas internas para almacenar estados

static int hoveredOptionHTP = -1;            // Opción sobre la que está el cursor
static int selectedOptionHTP = -1;           // Opción seleccionada por teclado
static Vector2 previousMousePositionHTP = { 0, 0 };
static bool usingMouseHTP = false;           // Flag: true = modo ratón, false = modo teclado

// Variable estática para almacenar la textura de la imagen

static Texture2D htpImage;

void InitHTP() 
{
    // Cargar la imagen "htp.png"

    htpImage = LoadTexture("htp.png");
}

void UnloadHTP() 
{
    // Descargar la imagen cuando ya no se necesite

    UnloadTexture(htpImage);
}

void DrawHTP(GameScreen* currentScreen)
{
    BeginDrawing();
    ClearBackground(BLACK);

    // Título y contenido

    int titleWidth = MeasureText("HOW TO PLAY", 50);
    DrawText("HOW TO PLAY", (GetScreenWidth() - titleWidth) / 2, 100, 50, WHITE);

    // Dibujar la imagen en lugar del texto

    int htpImageWidth = GetScreenWidth() / 2;
    int htpImageHeight = GetScreenHeight() / 2;


    DrawTexture(htpImage,  htpImageWidth/1.54, (GetScreenHeight() - htpImageHeight) -180, WHITE);

    // "Back" Button

    Color backColor = WHITE;

    // Resalta en rojo si el ratón está sobre "Back" o si está seleccionado por teclado

    if (hoveredOptionHTP == 0 || selectedOptionHTP == 0)
    {
        backColor = RED;
    }

    // Ajustar la posición del botón "Back" para que no se solape con la imagen

    DrawText("Back", 190, 900, 20, backColor);

    EndDrawing();

    // --- Actualización de input ---

    Vector2 mousePoint = GetMousePosition();

    // Umbral para evitar que pequeñas fluctuaciones activen el modo ratón

    float threshold = 0.1f;
    bool currentMouseMoved = (fabs(mousePoint.x - previousMousePositionHTP.x) > threshold ||
        fabs(mousePoint.y - previousMousePositionHTP.y) > threshold);
    previousMousePositionHTP = mousePoint;

    // Si se presiona una tecla de navegación, se activa el modo teclado

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))
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
        // --- MODO RATÓN ---

        selectedOptionHTP = -1;  // Limpiar la selección por teclado

        if (CheckCollisionPointRec(mousePoint, (Rectangle) { 190, 900, MeasureText("Back", 20), 20 }))
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
        // --- MODO TECLADO ---

        hoveredOptionHTP = -1;

        // Si aún no hay opción seleccionada, se asigna la única opción (índice 0)

        if (selectedOptionHTP < 0)
        {
            selectedOptionHTP = 0;
        }
        if (IsKeyPressed(KEY_ENTER) && selectedOptionHTP == 0)
        {
            *currentScreen = MENU;
        }
    }
}