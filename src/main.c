#include "raylib.h"
#include "resource_dir.h"
#include "menu.h"
#include "game.h"
#include "settings.h"
#include "htp.h"
#include "intro.h"
#include "discord_presence.h"
#define MAX_SOUNDS 10                   // ESTAS 3 LINEAS PARA PODER SUBIR EL VOLUMEN DEL SONIDITO START

Sound soundArray[MAX_SOUNDS] = { 0 };
int currentSound;

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Journey of The Prairie King");
    SearchAndSetResourceDir("resources");

    initDiscord();
    InitAudioDevice();                          // Inicializar el dispositivo de audio
    Sound fxStart = LoadSound("Sound_Start.wav");

    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);
    Texture JotPKLogo = LoadTexture("JotPK.png");

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    //ShowIntro(screenWidth, screenHeight, fxStart); // Pasar el sonido a la función de la intro

    GameScreen currentScreen = MENU;
    int selectedOption = 0;

    InitHTP();

    while (!WindowShouldClose())
    {
        updateDiscord(); // Llamar a updateDiscord en cada iteración del bucle principal

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

    UnloadHTP();
    UnloadSound(fxStart);
    CloseAudioDevice();
    UnloadTexture(JotPKLogo);
    CloseWindow();
    return 0;
}
