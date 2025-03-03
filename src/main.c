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
    InitWindow(GetScreenWidth(), GetScreenHeight(), "The Journey of the Prairie King");
    SearchAndSetResourceDir("resources");

    initDiscord();
    InitAudioDevice();                          // Inicializar el dispositivo de audio
    Sound fxStart = LoadSound("Sound_Start.wav");

    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);
    Texture JotPKLogo = LoadTexture("JotPK.png");

    // Cargar texturas del juego
    Texture2D Finn_Right = LoadTexture("Sprite_Sheet_Right.png");
    Texture2D Finn_Left = LoadTexture("Sprite_Sheet_Left.png");
    Texture2D Finn_Up = LoadTexture("Sprite_Sheet_Up.png");
    Texture2D Finn_Down = LoadTexture("Sprite_Sheet_Down.png");
    Texture2D Finn_Idle = LoadTexture("PJ_Idle.png");
    Texture2D Finn_Shooting_Right = LoadTexture("PJ_Idle_Right.png");
    Texture2D Finn_Shooting_Left = LoadTexture("PJ_Idle_Left.png");
    Texture2D Finn_Shooting_Up = LoadTexture("PJ_Idle_Up.png");
    Texture2D Finn_Shooting_Down = LoadTexture("PJ_Idle_Shoot_Down.png");
    Texture2D Bullet_1 = LoadTexture("Bullet_1.png");
    Texture2D Orc = LoadTexture("Sprite_Sheet_Orc.png");
    Texture2D backgroundSpriteSheet = LoadTexture("Sprite_Sheet_A1.png");

    Music BackgroundMusic_A1 = LoadMusicStream("BackgroundMusic_A1.mp3");
    SetMusicVolume(BackgroundMusic_A1, 0.1f);
    PlayMusicStream(BackgroundMusic_A1);

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
            DrawGame(Finn_Right, Finn_Left, Finn_Up, Finn_Down, Finn_Idle, Finn_Shooting_Right, Finn_Shooting_Left, Finn_Shooting_Up, Finn_Shooting_Down, Bullet_1, Orc, backgroundSpriteSheet, BackgroundMusic_A1);
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

    // Unload game textures
    UnloadTexture(Finn_Right);
    UnloadTexture(Finn_Left);
    UnloadTexture(Finn_Up);
    UnloadTexture(Finn_Down);
    UnloadTexture(Finn_Idle);
    UnloadTexture(Finn_Shooting_Right);
    UnloadTexture(Finn_Shooting_Left);
    UnloadTexture(Finn_Shooting_Up);
    UnloadTexture(Finn_Shooting_Down);
    UnloadTexture(Bullet_1);
    UnloadTexture(Orc);
    UnloadTexture(backgroundSpriteSheet);
    UnloadMusicStream(BackgroundMusic_A1);

    CloseWindow();
    return 0;
}
