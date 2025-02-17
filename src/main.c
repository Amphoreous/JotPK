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
    SetTargetFPS(60);
    SearchAndSetResourceDir("resources");
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
