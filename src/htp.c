#include "raylib.h"
#include "htp.h"
#include <math.h>  // Para fabs()

// Variables est�ticas internas para almacenar estados

static int hoveredOptionHTP = -1;            // Opci�n sobre la que est� el cursor
static int selectedOptionHTP = -1;           // Opci�n seleccionada por teclado
static Vector2 previousMousePositionHTP = { 0, 0 };
static bool usingMouseHTP = false;           // Flag: true = modo rat�n, false = modo teclado

// Variable est�tica para almacenar la textura de la imagen

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

    // T�tulo y contenido

    int titleWidth = MeasureText("HOW TO PLAY", 50);
    DrawText("HOW TO PLAY", (GetScreenWidth() - titleWidth) / 2, 100, 50, WHITE);

    // Dibujar la imagen en lugar del texto

    int htpImageWidth = GetScreenWidth() / 2;
    int htpImageHeight = GetScreenHeight() / 2;


    DrawTexture(htpImage,  htpImageWidth/1.54, (GetScreenHeight() - htpImageHeight) -180, WHITE);

    // "Back" Button

    Color backColor = WHITE;

    // Resalta en rojo si el rat�n est� sobre "Back" o si est� seleccionado por teclado

    if (hoveredOptionHTP == 0 || selectedOptionHTP == 0)
    {
        backColor = RED;
    }

    // Ajustar la posici�n del bot�n "Back" para que no se solape con la imagen

    DrawText("Back", 190, 900, 20, backColor);

    EndDrawing();

    // --- Actualizaci�n de input ---

    Vector2 mousePoint = GetMousePosition();

    // Umbral para evitar que peque�as fluctuaciones activen el modo rat�n

    float threshold = 0.1f;
    bool currentMouseMoved = (fabs(mousePoint.x - previousMousePositionHTP.x) > threshold ||
        fabs(mousePoint.y - previousMousePositionHTP.y) > threshold);
    previousMousePositionHTP = mousePoint;

    // Si se presiona una tecla de navegaci�n, se activa el modo teclado

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))
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
        // --- MODO RAT�N ---

        selectedOptionHTP = -1;  // Limpiar la selecci�n por teclado

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

        // Si a�n no hay opci�n seleccionada, se asigna la �nica opci�n (�ndice 0)

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