#include "raylib.h"
#include "resource_dir.h"
#include "menu.h"
#include "game.h"
#include "settings.h"
#include "htp.h"

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Journey of the Prairie King");

    SearchAndSetResourceDir("resources");

    // Cargar el icono de la ventana
    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon); // No se necesita más el objeto Image después de establecer el icono

    SetTargetFPS(60);
    Texture JotPKLogo = LoadTexture("JotPK.png");

    GameScreen currentScreen = MENU;
    int selectedOption = 0;

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

    UnloadTexture(JotPKLogo);
    CloseWindow();
    return 0;
}