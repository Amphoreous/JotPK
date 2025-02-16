#include "raylib.h"
#include "resource_dir.h"

void DrawLogoWithTransition(Texture logo, float alpha, int maxWidth, int maxHeight);

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Genshin Impact");
    SetTargetFPS(60);

    // Establecer el directorio de recursos
    SearchAndSetResourceDir("resources");

    // Cargar las imágenes de los logos
    Texture hoyoverseLogo = LoadTexture("hoyoverse.png");
    Texture genshinImpactLogo = LoadTexture("genshin_impact.png");

    float hoyoverseAlpha = 0.0f;
    float genshinImpactAlpha = 0.0f;
    float timeElapsed = 0.0f;

    // Definir el tamaño máximo para los logos
    int maxLogoWidth = 400;
    int maxLogoHeight = 200;

    while (!WindowShouldClose())
    {
        timeElapsed += GetFrameTime();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (timeElapsed < 1.0f)
        {
            // Aparecer el logo de Hoyoverse con transición
            hoyoverseAlpha = timeElapsed / 1.0f;
            DrawLogoWithTransition(hoyoverseLogo, hoyoverseAlpha, maxLogoWidth, maxLogoHeight);
        }
        else if (timeElapsed < 4.0f)
        {
            // Mantener el logo de Hoyoverse visible
            hoyoverseAlpha = 1.0f;
            DrawLogoWithTransition(hoyoverseLogo, hoyoverseAlpha, maxLogoWidth, maxLogoHeight);
        }
        else if (timeElapsed < 5.0f)
        {
            // Desaparecer el logo de Hoyoverse con transición
            hoyoverseAlpha = 1.0f - ((timeElapsed - 4.0f) / 1.0f);
            DrawLogoWithTransition(hoyoverseLogo, hoyoverseAlpha, maxLogoWidth, maxLogoHeight);
        }
        else if (timeElapsed < 6.0f)
        {
            // Pausa antes de mostrar el logo de Genshin Impact
        }
        else if (timeElapsed < 7.0f)
        {
            // Aparecer el logo de Genshin Impact con transición
            genshinImpactAlpha = (timeElapsed - 6.0f) / 1.0f;
            DrawLogoWithTransition(genshinImpactLogo, genshinImpactAlpha, maxLogoWidth, maxLogoHeight);
        }
        else if (timeElapsed < 12.0f)
        {
            // Mantener el logo de Genshin Impact visible
            genshinImpactAlpha = 1.0f;
            DrawLogoWithTransition(genshinImpactLogo, genshinImpactAlpha, maxLogoWidth, maxLogoHeight);
        }
        else if (timeElapsed < 13.0f)
        {
            // Desaparecer el logo de Genshin Impact con transición
            genshinImpactAlpha = 1.0f - ((timeElapsed - 12.0f) / 1.0f);
            DrawLogoWithTransition(genshinImpactLogo, genshinImpactAlpha, maxLogoWidth, maxLogoHeight);
        }
        else
        {
            // Finalizar la secuencia de introducción
            DrawText("Hello World!", 190, 200, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    // Descargar las texturas
    UnloadTexture(hoyoverseLogo);
    UnloadTexture(genshinImpactLogo);

    CloseWindow();
    return 0;
}

void DrawLogoWithTransition(Texture logo, float alpha, int maxWidth, int maxHeight)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int logoWidth = logo.width;
    int logoHeight = logo.height;

    // Redimensionar el logo si es más grande que el tamaño máximo permitido
    if (logoWidth > maxWidth || logoHeight > maxHeight)
    {
        float aspectRatio = (float)logoWidth / (float)logoHeight;
        if (logoWidth > maxWidth)
        {
            logoWidth = maxWidth;
            logoHeight = (int)(maxWidth / aspectRatio);
        }
        if (logoHeight > maxHeight)
        {
            logoHeight = maxHeight;
            logoWidth = (int)(maxHeight * aspectRatio);
        }
    }

    // Calcular la posición para centrar el logo
    int posX = (screenWidth - logoWidth) / 2;
    int posY = (screenHeight - logoHeight) / 2;

    // Dibujar el logo con la transparencia especificada
    DrawTexturePro(logo, (Rectangle) { 0, 0, logo.width, logo.height }, (Rectangle) { posX, posY, logoWidth, logoHeight }, (Vector2) { 0, 0 }, 0.0f, Fade(WHITE, alpha));
}
