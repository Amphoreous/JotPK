#pragma once
#include "Game.hpp"
#include "AssetManager.hpp"

class GameApplication {
public:
    GameApplication();
    ~GameApplication();

    void Initialize();
    void Run();
    void Shutdown();

private:
    AssetManager m_assets;
    std::unique_ptr<Game> m_game;
    Vector2 m_pixelScale;
    bool m_isRunning;
};
