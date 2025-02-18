#include "intro.h"

void FadeInOutLogo(Texture2D texture, const char* companyText, int fadeDuration, int screenWidth, int screenHeight) {
    int fadeInDuration = fadeDuration / 2;
    int fadeOutDuration = fadeDuration / 2;
    int pauseDuration = 180;
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

void FadeInOutTeam(const char* teamMembers, int fadeDuration, int screenWidth, int screenHeight) {
    int fadeInDuration = fadeDuration / 2;
    int fadeOutDuration = fadeDuration / 2;
    int pauseDuration = 180;
    int totalDuration = fadeInDuration + pauseDuration + fadeOutDuration;

    int teamTextSize = 40;
    int teamTextX = (screenWidth - MeasureText(teamMembers, teamTextSize)) / 2;
    int teamTextY = (screenHeight - teamTextSize) / 2;

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
        DrawText(teamMembers, teamTextX, teamTextY, teamTextSize, Fade(WHITE, alpha));
        EndDrawing();

        framesCounter++;
    }
}

void ShowIntro(int screenWidth, int screenHeight) {
    Texture2D icon = LoadTexture("icon.png");
    const char* companyName = "Amphoreous";
    const char* teamMembers = "Zakaria Hamdaoui, Sofia Giner Vargas, Joel Martinez Arjona";
    int fadeDuration = 240;

    FadeInOutLogo(icon, companyName, fadeDuration, screenWidth, screenHeight);
    FadeInOutTeam(teamMembers, fadeDuration, screenWidth, screenHeight);

    UnloadTexture(icon);
}