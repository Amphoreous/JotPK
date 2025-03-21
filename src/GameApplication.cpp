#include "GameApplication.hpp"
#include "resource_dir.h"

GameApplication::GameApplication() : m_isRunning(false) {}

GameApplication::~GameApplication() {
    Shutdown();
}

void GameApplication::Initialize() {
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MAXIMIZED | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    SearchAndSetResourceDir("resources");
    
    // Cargar y preparar el icono antes de crear la ventana
    Image icon = LoadImage("cursors/cursors.png");
    ImageColorReplace(&icon, BLANK, BLANK);  // Asegurar transparencia correcta
    
    // Recortar y escalar el icono de la compañía
    Rectangle iconRegion = { 432, 1808, 16, 16 };
    Image iconCrop = ImageFromImage(icon, iconRegion);
    UnloadImage(icon);
    
    // Crear múltiples tamaños para mejor compatibilidad con Windows
    Image icon16 = ImageCopy(iconCrop);
    Image icon32 = ImageCopy(iconCrop);
    Image icon48 = ImageCopy(iconCrop);
    Image icon64 = ImageCopy(iconCrop);
    Image icon128 = ImageCopy(iconCrop);
    
    ImageResize(&icon16, 16, 16);
    ImageResize(&icon32, 32, 32);
    ImageResize(&icon48, 48, 48);
    ImageResize(&icon64, 64, 64);
    ImageResize(&icon128, 128, 128);
    
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Journey of the Prairie King");
    
    // Establecer iconos de menor a mayor tamaño
    SetWindowIcon(icon16);
    SetWindowIcon(icon32);
    SetWindowIcon(icon48);
    SetWindowIcon(icon64);
    SetWindowIcon(icon128);
    
    // Limpiar recursos
    UnloadImage(iconCrop);
    UnloadImage(icon16);
    UnloadImage(icon32);
    UnloadImage(icon48);
    UnloadImage(icon64);
    UnloadImage(icon128);
    
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    m_pixelScale.x = (float)(int)(screenWidth / 320);
    m_pixelScale.y = (float)(int)(screenHeight / 180);
    m_pixelScale.x = m_pixelScale.y = fminf(m_pixelScale.x, m_pixelScale.y);
    
    m_assets.LoadAssets();
    
    Texture2D cursorsTexture = m_assets.GetTexture("cursors");
    if (cursorsTexture.id > 0) {
        SetTextureFilter(cursorsTexture, TEXTURE_FILTER_POINT);
    }
    
    m_game = std::make_unique<Game>(m_assets, m_pixelScale);
    m_isRunning = true;
}

void GameApplication::Run() {
    while (m_isRunning && !WindowShouldClose()) {
        m_game->Update(GetFrameTime());
        m_game->Draw();
    }
    
    // Realizar la limpieza aquí, antes de que termine el programa
    m_game.reset();  // Primero liberamos el juego
    m_assets.UnloadAssets();  // Luego los assets
    CloseWindow();  // Finalmente cerramos la ventana
}

void GameApplication::Shutdown() {
    
}