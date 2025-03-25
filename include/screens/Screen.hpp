#pragma once
#include "../AssetManager.hpp"
#include "raylib.h"

class Screen {
public:
    Screen(AssetManager& assets, const Vector2& pixelScale) 
        : m_assets(assets), m_pixelScale(pixelScale), m_isFinished(false) {}
    virtual ~Screen() = default;
    
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
    virtual bool IsFinished() const { return m_isFinished; }

protected:
    AssetManager& m_assets;
    Vector2 m_pixelScale;
    bool m_isFinished;
};
