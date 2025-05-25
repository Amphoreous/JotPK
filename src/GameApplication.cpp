#include "GameApplication.hpp"
#include "resource_dir.h"
#include "discord/DiscordManager.hpp"

GameApplication::GameApplication() : m_isRunning(false) {}

GameApplication::~GameApplication() {
    Shutdown();
}

void GameApplication::Initialize() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

    SearchAndSetResourceDir("resources");
        
    // Get the monitor's refresh rate for proper vsync
    SetTargetFPS(60);
    
    // Get current monitor dimensions for fullscreen mode
    int display = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(display);
    int monitorHeight = GetMonitorHeight(display);
    
    // Initialize window with monitor dimensions
    InitWindow(monitorWidth, monitorHeight, "Journey of the Prairie King");
    
    // Set position to top-left corner before enabling fullscreen
    SetWindowPosition(0, 0);
    
    // Enable fullscreen mode
    ToggleFullscreen();
        
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    m_pixelScale.x = (float)(int)(screenWidth / 320);
    m_pixelScale.y = (float)(int)(screenHeight / 180);
    m_pixelScale.x = m_pixelScale.y = fminf(m_pixelScale.x, m_pixelScale.y);
    
    m_assets.LoadAssets();
    
    // Set point filtering for pixel art
    Texture2D cursorsTexture = m_assets.GetTexture("cursors");
    if (cursorsTexture.id > 0) {
        SetTextureFilter(cursorsTexture, TEXTURE_FILTER_POINT);
    }
    
    m_game = std::make_unique<Game>(m_assets, m_pixelScale);
    m_isRunning = true;

    // Initialize Discord integration
    DiscordManager::Initialize();
}

void GameApplication::Run() {
    while (m_isRunning && !WindowShouldClose()) {
        // Update Discord Rich Presence
        DiscordManager::Update();
        
        m_game->Update(GetFrameTime());
        m_game->Draw();
    }
    
    // Cleanup order is important
    DiscordManager::Shutdown();
    m_game.reset();
    m_assets.UnloadAssets();
    CloseWindow();
}

void GameApplication::Shutdown() {
    
}
