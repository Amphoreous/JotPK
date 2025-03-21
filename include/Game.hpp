#pragma once
#include "raylib.h"
#include "AssetManager.hpp"
#include "screens/Screen.hpp"
#include <memory>

enum class GameState {
    Intro,
    Menu,
    Playing
};

class Game {
public:
    Game(AssetManager& assets, const Vector2& pixelScale);
    ~Game();

    void Update(float deltaTime);
    void Draw();

private:
    AssetManager& m_assets;
    Vector2 m_pixelScale;
    GameState m_state;
    std::unique_ptr<Screen> m_currentScreen;
};
