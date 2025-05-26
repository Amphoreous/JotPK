#pragma once
#include "raylib.h"
#include <unordered_map>
#include <string>

class AssetManager {
public:
    void LoadAssets();
    void UnloadAssets();
    
    Texture2D GetTexture(const std::string& name) const;
    Rectangle GetTextureRegion(const std::string& name, int x, int y, int width, int height) const;
    Sound GetSound(const std::string& name) const;
    Font GetFont(const std::string& name) const;
    
    // Add music functions
    Music GetMusic(const std::string& name) const;
    void LoadMusic(const std::string& name, const std::string& path);
    void UnloadMusic(const std::string& name);

private:
    std::unordered_map<std::string, Texture2D> m_textures;
    std::unordered_map<std::string, Sound> m_sounds;
    std::unordered_map<std::string, Font> m_fonts;
    std::unordered_map<std::string, Music> m_music;
    void LoadFonts();
};
