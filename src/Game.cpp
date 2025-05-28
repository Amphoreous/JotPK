#include "Game.hpp"
#include "screens/IntroScreen.hpp"
#include "screens/MenuScreen.hpp"
#include "screens/SettingsScreen.hpp"
#include "screens/HowToPlayScreen.hpp"
#include "screens/GameplayScreen.hpp"

Game::Game(AssetManager& assets, const Vector2& pixelScale)
    : m_assets(assets), m_pixelScale(pixelScale), m_state(GameState::Intro) {
    m_currentScreen = std::make_unique<IntroScreen>(assets, pixelScale);
}

Game::~Game() = default;

void Game::Update(float deltaTime) {
    // Before creating a new screen, ensure the current one is properly finished
    if (m_currentScreen && m_currentScreen->IsFinished()) {
        // Create the new screen and set it as current
        if (m_state == GameState::Intro) {
            m_state = GameState::Menu;
            m_currentScreen = nullptr; // Clear current screen first
            m_currentScreen = std::make_unique<MenuScreen>(m_assets, m_pixelScale);
        }
        else if (auto* menuScreen = dynamic_cast<MenuScreen*>(m_currentScreen.get())) {
            switch (menuScreen->GetSelectedOption()) {
                case 0: // Start Game
                    m_state = GameState::Playing;
                    m_currentScreen = nullptr; // Clear current screen first
                    m_currentScreen = std::make_unique<GameplayScreen>(m_assets, m_pixelScale);
                    break;
                case 1: // How To Play (cambiado el orden)
                    m_currentScreen = std::make_unique<HowToPlayScreen>(m_assets, m_pixelScale);
                    break;
                case 2: // Settings (cambiado el orden)
                    m_currentScreen = std::make_unique<SettingsScreen>(m_assets, m_pixelScale);
                    break;
                case 3: // Exit
                    CloseWindow();
                    break;
            }
        }
        else if (dynamic_cast<SettingsScreen*>(m_currentScreen.get()) ||
                 dynamic_cast<HowToPlayScreen*>(m_currentScreen.get())) {
            m_currentScreen = std::make_unique<MenuScreen>(m_assets, m_pixelScale);
        }
        else if (auto* gameplayScreen = dynamic_cast<GameplayScreen*>(m_currentScreen.get())) {
            // Handle return from gameplay screen
            m_state = GameState::Menu;
            m_currentScreen = nullptr; // Clear current screen first
            m_currentScreen = std::make_unique<MenuScreen>(m_assets, m_pixelScale);
        }
    }
    
    if (m_currentScreen) {
        m_currentScreen->Update(deltaTime);
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    m_currentScreen->Draw();
    EndDrawing();
}
