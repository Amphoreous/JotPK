#include <raylib.h>
#include <iostream>
#include <string>
#include "resource_dir.h"
#include "resource_manager.h"
#include "menu.h"
#include "settings.h"
#include "htp.h"
#include "intro.h"
#include "discord/discord_manager.h"
#include "game/game_state.h"
#include "game/assets_manager.h"

#define MAX_SOUNDS 10

// Global variables
Sound soundArray[MAX_SOUNDS] = { 0 };
int currentSound;
GameStateManager* gameManager = nullptr;

// Variable global para almacenar el callback de cambio de tamaño
void (*windowResizeCallback)() = NULL;

// Function to load all game resources
void LoadGameResources();

// Function to unload all game resources
void UnloadGameResources();

// Add this near the top with other function declarations
// void DebugDrawAssets();

// Función para establecer el callback
void SetWindowResizeCallback(void (*callback)()) {
    windowResizeCallback = callback;
}

// Callback para cuando la ventana cambia de tamaño
void WindowResizeCallback() {
    // Notificar al game state manager del cambio de tamaño
    if (gameManager != nullptr) {
        gameManager->HandleScreenSizeChange();
    }
}

int main() {
    // Initialize window
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "The Journey of the Prairie King");
    SearchAndSetResourceDir("resources");

    // Initialize Discord Rich Presence
    bool discordInitialized = discord_sdk::initialize();
    if (!discordInitialized) {
        std::cerr << "Error initializing Discord" << std::endl;
    }

    // Initialize audio
    InitAudioDevice();
    Sound fxStart = LoadSound("Sound_Start.wav");

    // Set window icon
    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);

    // Initialize assets manager
    AssetsManager::getInstance().initialize();

    // Load game resources
    Texture JotPKLogo = LoadTexture("JotPK.png");
    LoadGameResources();

    // Create game state manager
    gameManager = new GameStateManager();
    gameManager->Initialize();

    // Background music
    Music Overworld = LoadMusicStream("Audio/Music/Journey Of The Prairie King - Overworld.mp3");
    SetMusicVolume(Overworld, 1.0f);
    PlayMusicStream(Overworld);

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Uncomment to show intro
    // ShowIntro(screenWidth, screenHeight, fxStart);

    GameScreen currentScreen = MENU;
    int selectedOption = 0;

    InitHTP();

    // Configura el callback
    SetWindowResizeCallback(WindowResizeCallback);

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Update Discord status
        if (discordInitialized) {
            discord_sdk::update();
        }

        // Update music
        UpdateMusicStream(Overworld);

        // Add a hotkey to force switch to GAME
        if (IsKeyPressed(KEY_F5)) {
            currentScreen = GAME;
            if (gameManager) {
                delete gameManager;
                gameManager = new GameStateManager();
                gameManager->Initialize();
            }
        }

        static int prevScreenWidth = GetScreenWidth();
        static int prevScreenHeight = GetScreenHeight();

        // Check if screen size changed
        if (prevScreenWidth != GetScreenWidth() || prevScreenHeight != GetScreenHeight()) {
            prevScreenWidth = GetScreenWidth();
            prevScreenHeight = GetScreenHeight();
            
            // Call the callback if it's set
            if (windowResizeCallback != NULL) {
                windowResizeCallback();
            }
        }

        // Handle different game screens
        switch (currentScreen) {
            case MENU:
                DrawMenu(JotPKLogo, selectedOption);
                UpdateMenu(&currentScreen, &selectedOption);
                break;
                
            case GAME:
                // Update game state
                gameManager->Update(deltaTime);
                
                // Draw game
                BeginDrawing();
                ClearBackground(BLACK);
                gameManager->Draw();
                
                // Handle game state transitions
                if (gameManager->GetCurrentState() == GAME_OVER) {
                    if (IsKeyPressed(KEY_R)) {
                        // Reset game
                        delete gameManager;
                        gameManager = new GameStateManager();
                        gameManager->Initialize();
                        PlayMusicStream(Overworld);
                    }
                }
                
                EndDrawing();
                break;
                
            case SETTINGS:
                DrawSettings(&currentScreen);
                break;
                
            case HTP:
                DrawHTP(&currentScreen);
                break;
        }
    }

    // Clean up resources
    UnloadHTP();
    UnloadSound(fxStart);
    UnloadTexture(JotPKLogo);
    UnloadMusicStream(Overworld);
    UnloadGameResources();
    
    if (gameManager) {
        delete gameManager;
        gameManager = nullptr;
    }

    // Shutdown Discord Rich Presence
    if (discordInitialized) {
        discord_sdk::shutdown();
    }

    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

// Resource management functions
void LoadGameResources() {
    // Just load the single sprite sheet using AssetsManager
    // Already handled in AssetsManager::initialize()
}

void UnloadGameResources() {
    // Unload the sprite sheet
    AssetsManager::getInstance().unload();
}
