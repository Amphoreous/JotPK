#include "raylib.h"
#include "resource_dir.h"
#include "menu.h"
#include "game.h"
#include "settings.h"
#include "htp.h"
#include "intro.h"

int main(void) 
{
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Journey of the Prairie King");

    InitAudioDevice();                          // ESTA LINEA Y LA DE ABAJO JUSTO SON PARA EL SONIDITO ESTAN BIEN PUESTAS AQUI.
    Sound fxStart = LoadSound("resources/Sound_Start.wav");
    if (IsKeyPressed(KEY_SPACE)) PlaySound(fxStart); // ESTA TERCERA SE TIENE QUE PONER EN INTRO (SUPONGO) PERO NO SE PUEDE PORQUE NO ESTARA 'DEFINIDA'
                                                     // SE DEFINE JUSTO EN LA ANTERIOR

    SearchAndSetResourceDir("resources");

    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);
    Texture JotPKLogo = LoadTexture("JotPK.png");

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    ShowIntro(screenWidth, screenHeight); // Llamar a la función de la intro

    GameScreen currentScreen = MENU;
    int selectedOption = 0;

    // Inicializar la imagen de HTP

    InitHTP();

    while (!WindowShouldClose()) 
    {
        if (currentScreen == MENU) 
        {
            DrawMenu(JotPKLogo, selectedOption);
            UpdateMenu(&currentScreen, &selectedOption);
        }
        else if (currentScreen == GAME) 
        {
            DrawGame();
        }
        else if (currentScreen == SETTINGS) 
        {
            DrawSettings(&currentScreen);
        }
        else if (currentScreen == HTP) 
        {
            DrawHTP(&currentScreen);
        }



    }

    // Descargar la imagen de HTP

    UnloadHTP();

    UnloadSound(fxStart);
    CloseAudioDevice();

    UnloadTexture(JotPKLogo);
    CloseWindow();
    return 0;
}