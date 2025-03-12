#include "raylib.h"
#include "raymath.h"  // Add this for Clamp function
#include "settings.h"
#include "resource_manager.h"
#include <cmath>  // For fabs()

// Settings state variables
static int hoveredOptionSettings = -1;            // Option the cursor is hovering over
static int selectedOptionSettings = -1;           // Option selected via keyboard
static Vector2 previousMousePositionSettings = { 0, 0 };
static bool usingMouseSettings = false;           // Flag: true = mouse mode, false = keyboard mode

// Settings values
static bool musicEnabled = true;
static bool soundEnabled = true;
static int musicVolume = 80;
static int soundVolume = 80;
static bool fullScreen = false;

// Define settings options
#define SETTINGS_OPTIONS 5
static const char* settingsLabels[SETTINGS_OPTIONS] = {
    "Music",
    "Sound Effects",
    "Music Volume",
    "Sound Volume",
    "Fullscreen"
};

void DrawSettings(GameScreen* currentScreen)
{
    const int titleFontSize = 50;
    const int optionFontSize = 24;
    const int spacing = 60;
    
    BeginDrawing();
    ClearBackground(BLACK);

    // Title
    int titleWidth = MeasureText("SETTINGS", titleFontSize);
    DrawText("SETTINGS", (GetScreenWidth() - titleWidth) / 2, 100, titleFontSize, WHITE);

    // Draw options
    int startY = 220;
    for (int i = 0; i < SETTINGS_OPTIONS; i++) {
        Color optionColor = (hoveredOptionSettings == i || selectedOptionSettings == i) ? RED : WHITE;
        DrawText(settingsLabels[i], 190, startY + i * spacing, optionFontSize, optionColor);
        
        // Draw option values/toggles
        switch (i) {
            case 0: // Music toggle
                DrawText(musicEnabled ? "ON" : "OFF", GetScreenWidth() - 300, startY + i * spacing, 
                    optionFontSize, optionColor);
                break;
                
            case 1: // Sound effects toggle
                DrawText(soundEnabled ? "ON" : "OFF", GetScreenWidth() - 300, startY + i * spacing, 
                    optionFontSize, optionColor);
                break;
                
            case 2: // Music volume
                {
                    // Volume slider
                    Rectangle sliderBg = { GetScreenWidth() - 400, startY + i * spacing + optionFontSize/2 - 5, 300, 10 };
                    DrawRectangleRec(sliderBg, DARKGRAY);
                    
                    // Active part of slider
                    Rectangle sliderActive = { sliderBg.x, sliderBg.y, sliderBg.width * (musicVolume / 100.0f), sliderBg.height };
                    DrawRectangleRec(sliderActive, optionColor);
                    
                    // Volume percentage
                    DrawText(TextFormat("%d%%", musicVolume), GetScreenWidth() - 80, startY + i * spacing, optionFontSize, optionColor);
                }
                break;
                
            case 3: // Sound volume
                {
                    // Volume slider
                    Rectangle sliderBg = { GetScreenWidth() - 400, startY + i * spacing + optionFontSize/2 - 5, 300, 10 };
                    DrawRectangleRec(sliderBg, DARKGRAY);
                    
                    // Active part of slider
                    Rectangle sliderActive = { sliderBg.x, sliderBg.y, sliderBg.width * (soundVolume / 100.0f), sliderBg.height };
                    DrawRectangleRec(sliderActive, optionColor);
                    
                    // Volume percentage
                    DrawText(TextFormat("%d%%", soundVolume), GetScreenWidth() - 80, startY + i * spacing, optionFontSize, optionColor);
                }
                break;
                
            case 4: // Fullscreen toggle
                DrawText(fullScreen ? "ON" : "OFF", GetScreenWidth() - 300, startY + i * spacing, 
                    optionFontSize, optionColor);
                break;
        }
    }

    // Back button
    Rectangle backButtonRect = { 190, GetScreenHeight() - 100, (float)MeasureText("Back", optionFontSize), optionFontSize };
    Color backColor = (hoveredOptionSettings == SETTINGS_OPTIONS || selectedOptionSettings == SETTINGS_OPTIONS) ? RED : WHITE;
    DrawText("Back", 190, GetScreenHeight() - 100, optionFontSize, backColor);

    // Controls info
    DrawText("Use ↑/↓ to navigate, ←/→ to change settings, ENTER to confirm", 
             190, GetScreenHeight() - 50, 16, GRAY);

    EndDrawing();

    // --- Input handling ---
    Vector2 mousePoint = GetMousePosition();

    // Mouse movement detection
    float threshold = 0.1f;
    bool currentMouseMoved = (std::fabs(mousePoint.x - previousMousePositionSettings.x) > threshold ||
                             std::fabs(mousePoint.y - previousMousePositionSettings.y) > threshold);
    previousMousePositionSettings = mousePoint;

    // Switch control scheme based on input
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) || 
        IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
        usingMouseSettings = false;
    }

    if (currentMouseMoved) {
        usingMouseSettings = true;
    }

    if (usingMouseSettings) {
        // Mouse controls
        selectedOptionSettings = -1;
        hoveredOptionSettings = -1;

        // Check if mouse is hovering over options
        for (int i = 0; i < SETTINGS_OPTIONS; i++) {
            Rectangle optionRect = { 190, static_cast<float>(startY + i * spacing), 
                                    static_cast<float>(MeasureText(settingsLabels[i], optionFontSize)), 
                                    static_cast<float>(optionFontSize) };
            
            if (CheckCollisionPointRec(mousePoint, optionRect)) {
                hoveredOptionSettings = i;
                
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    switch (i) {
                        case 0: // Music toggle
                            musicEnabled = !musicEnabled;
                            // TODO: Apply music setting
                            break;
                        case 1: // Sound toggle
                            soundEnabled = !soundEnabled;
                            // TODO: Apply sound setting
                            break;
                        case 4: // Fullscreen toggle
                            fullScreen = !fullScreen;
                            if (fullScreen)
                                ToggleFullscreen();
                            else
                                ToggleFullscreen();
                            break;
                    }
                }
            }
            if (i == 2 || i == 3) { // Volume sliders
                Rectangle sliderRect = { GetScreenWidth() - 400, startY + i * spacing + optionFontSize/2 - 5, 300, 10 };
                
                if (CheckCollisionPointRec(mousePoint, 
                                          Rectangle{sliderRect.x - 10, sliderRect.y - 10, sliderRect.width + 20, sliderRect.height + 20})) {
                    hoveredOptionSettings = i;
                    
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                        // Calculate new volume based on mouse position
                        float relativeX = (mousePoint.x - sliderRect.x) / sliderRect.width;
                        int newVolume = (int)(Clamp(relativeX, 0.0f, 1.0f) * 100.0f);
                        
                        if (i == 2) // Music volume
                            musicVolume = newVolume;
                        else        // Sound volume
                            soundVolume = newVolume;
                    }
                }
            }
        }

        // Check if back button is hovered/clicked
        Rectangle backButtonRect = { 190, GetScreenHeight() - 100, (float)MeasureText("Back", optionFontSize), optionFontSize };
        if (CheckCollisionPointRec(mousePoint, backButtonRect)) {
            hoveredOptionSettings = SETTINGS_OPTIONS;
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                *currentScreen = MENU;
                // TODO: Save settings
            }
        }
    } else {
        // Keyboard controls
        if (selectedOptionSettings < 0) {
            selectedOptionSettings = 0;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            selectedOptionSettings = (selectedOptionSettings + 1) % (SETTINGS_OPTIONS + 1);
        }
        if (IsKeyPressed(KEY_UP)) {
            selectedOptionSettings = (selectedOptionSettings + SETTINGS_OPTIONS) % (SETTINGS_OPTIONS + 1);
        }
        if (selectedOptionSettings < SETTINGS_OPTIONS) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
                switch (selectedOptionSettings) {
                    case 0: // Music toggle
                        musicEnabled = !musicEnabled;
                        // TODO: Apply music setting
                        break;
                    case 1: // Sound toggle
                        soundEnabled = !soundEnabled;
                        // TODO: Apply sound setting
                        break;
                    case 2: // Music volume
                        if (IsKeyPressed(KEY_LEFT))
                            musicVolume = (int)Clamp(musicVolume - 10, 0, 100);
                        else
                            musicVolume = (int)Clamp(musicVolume + 10, 0, 100);
                        break;
                    case 3: // Sound volume
                        if (IsKeyPressed(KEY_LEFT))
                            soundVolume = (int)Clamp(soundVolume - 10, 0, 100);
                        else
                            soundVolume = (int)Clamp(soundVolume + 10, 0, 100);
                        break;
                    case 4: // Fullscreen toggle
                        fullScreen = !fullScreen;
                        if (fullScreen)
                            ToggleFullscreen();
                        else
                            ToggleFullscreen();
                        break;
                }
            }
        }
        if (IsKeyPressed(KEY_ENTER) && selectedOptionSettings == SETTINGS_OPTIONS) {
            *currentScreen = MENU;
            // TODO: Save settings
        }
    }
}

// Function to save settings to a file
void SaveSettings() {
    // Implementation would go here
}

// Function to load settings from a file
void LoadSettings() {
    // Implementation would go here
}