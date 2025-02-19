#include "intro.h"

void FadeInOutLogo(Texture2D texture, const char* companyText, int fadeDuration, int screenWidth, int screenHeight) {
    int fadeInDuration = fadeDuration / 2;
    int fadeOutDuration = fadeDuration / 2;
    int pauseDuration = 90;
    int totalDuration = fadeInDuration + pauseDuration + fadeOutDuration;

    int companyTextSize = 60;
    int marginBetweenLogoAndText = 20;

    int maxLogoHeight = screenHeight - companyTextSize - marginBetweenLogoAndText;
    float logoScale = 1.0f;
    if (texture.height > maxLogoHeight) {
        logoScale = (float)maxLogoHeight / texture.height;
    }
    int logoDrawWidth = texture.width * logoScale;
    int logoDrawHeight = texture.height * logoScale;

    int headerContentHeight = logoDrawHeight + marginBetweenLogoAndText + companyTextSize;
    int headerContentY = (screenHeight - headerContentHeight) / 2;
    int logoY = headerContentY;
    int logoX = (screenWidth - logoDrawWidth) / 2;

    int companyTextX = (screenWidth - MeasureText(companyText, companyTextSize)) / 2;
    int companyTextY = logoY + logoDrawHeight + marginBetweenLogoAndText;

    int framesCounter = 0;
    while (framesCounter < totalDuration) {
        float alpha;
        if (framesCounter < fadeInDuration) {
            alpha = (float)framesCounter / fadeInDuration;
        }
        else if (framesCounter < fadeInDuration + pauseDuration) {
            alpha = 1.0f;
        }
        else {
            alpha = 1.0f - (float)(framesCounter - fadeInDuration - pauseDuration) / fadeOutDuration;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        if (texture.id != 0) {
            DrawTextureEx(texture, (Vector2) { logoX, logoY }, 0.0f, logoScale, Fade(WHITE, alpha));
        }
        DrawText(companyText, companyTextX, companyTextY, companyTextSize, Fade(WHITE, alpha));
        EndDrawing();

        framesCounter++;
    }
}

void FadeInOutText(const char* text, int textSize, int fadeDuration, int screenWidth, int screenHeight) {
    int fadeInDuration = fadeDuration / 2;
    int fadeOutDuration = fadeDuration / 2;
    int pauseDuration = 90;
    int totalDuration = fadeInDuration + pauseDuration + fadeOutDuration;

    int textX = (screenWidth - MeasureText(text, textSize)) / 2;
    int textY = (screenHeight - textSize) / 2;

    int framesCounter = 0;
    while (framesCounter < totalDuration) {
        float alpha;
        if (framesCounter < fadeInDuration) {
            alpha = (float)framesCounter / fadeInDuration;
        }
        else if (framesCounter < fadeInDuration + pauseDuration) {
            alpha = 1.0f;
        }
        else {
            alpha = 1.0f - (float)(framesCounter - fadeInDuration - pauseDuration) / fadeOutDuration;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(text, textX, textY, textSize, Fade(WHITE, alpha));
        EndDrawing();

        framesCounter++;
    }
}

void ShowIntro(int screenWidth, int screenHeight) {
    Texture2D icon = LoadTexture("icon.png");
    Texture2D JotPKLogo = LoadTexture("JotPK.png");
    Texture2D pressStart = LoadTexture("press_space.png");
    const char* companyName = "Amphoreous";
    const char* teamMembers = "Zakaria Hamdaoui\nSofia Giner Vargas\nJoel Martinez Arjona";
    const char* copyrightText = "Copyright. Amphoreous 2025. Todos los derechos reservados.";
    int fadeDuration = 180;

    FadeInOutLogo(icon, companyName, fadeDuration, screenWidth, screenHeight);
    FadeInOutText(teamMembers, 30, fadeDuration, screenWidth, screenHeight);

    // Mostrar el logo de JotPK y el texto de "press_start"
    int logoWidth = screenWidth / 2;
    int logoHeight = (JotPKLogo.height * logoWidth) / JotPKLogo.width;
    int logoX = (screenWidth - logoWidth) / 2;
    int logoY = (screenHeight - 100) / 2 - (logoHeight / 2);

    int pressStartWidth = pressStart.width;
    int pressStartHeight = pressStart.height;
    int pressStartX = (screenWidth - pressStartWidth) / 2;
    int pressStartY = logoHeight - 60;

    int copyrightTextSize = 20;
    int copyrightTextX = (screenWidth - MeasureText(copyrightText, copyrightTextSize)) / 2;
    int copyrightTextY = screenHeight - copyrightTextSize - 10;

    bool showPressStart = true;
    int framesCounter = 0;

    while (!IsKeyPressed(KEY_SPACE)) {
        framesCounter++;
        if (framesCounter % 60 < 30) {
            showPressStart = true;
        }
        else {
            showPressStart = false;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(JotPKLogo, (Vector2) { logoX, logoY }, 0.0f, (float)logoWidth / JotPKLogo.width, WHITE);
        if (showPressStart) {
            DrawTexture(pressStart, pressStartX, pressStartY, WHITE);
        }
        DrawText(copyrightText, copyrightTextX, copyrightTextY, copyrightTextSize, WHITE);
        EndDrawing();
    }

    // Parpadeo rápido de "press_start" al presionar espacio
    for (int i = 0; i < 20; i++) { // Aumentar el número de iteraciones para que dure más tiempo
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(JotPKLogo, (Vector2) { logoX, logoY }, 0.0f, (float)logoWidth / JotPKLogo.width, WHITE);
        if (i % 2 == 0) {
            DrawTexture(pressStart, pressStartX, pressStartY, WHITE);
        }
        EndDrawing();
        WaitTime(0.1); // Esperar 0.1 segundos
    }

    // Pausa en negro durante 1.5 segundos con el logo de JotPK
    for (int i = 0; i < 15; i++) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(JotPKLogo, (Vector2) { logoX, logoY }, 0.0f, (float)logoWidth / JotPKLogo.width, WHITE);
        EndDrawing();
        WaitTime(0.1); // Esperar 0.1 segundos
    }

    UnloadTexture(icon);
    UnloadTexture(JotPKLogo);
    UnloadTexture(pressStart);
}