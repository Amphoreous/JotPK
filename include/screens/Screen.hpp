#pragma once
#include "raylib.h"
#include "../AssetManager.hpp"

class Screen {
public:
    virtual ~Screen() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
    
    bool IsFinished() const { return m_isFinished; }
    
protected:
    bool m_isFinished = false;
    AssetManager& m_assets;
    Vector2 m_pixelScale;
    
    Screen(AssetManager& assets, const Vector2& pixelScale) 
        : m_assets(assets), m_pixelScale(pixelScale) {}
};
