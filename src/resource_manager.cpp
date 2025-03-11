#include "resource_manager.h"
#include <iostream>

Texture2D ResourceManager::loadTexture(const std::string& name, const std::string& path) {
    // Check if already loaded
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    
    // Load the texture
    Texture2D texture = LoadTexture(path.c_str());
    textures[name] = texture;
    
    return texture;
}

Texture2D ResourceManager::loadTexture(const std::string& name, const std::string& path, Texture2D texture) {
    // Store an already loaded texture
    textures[name] = texture;
    return texture;
}

Texture2D ResourceManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    
    // Return empty texture if not found
    Texture2D empty = {0};
    return empty;
}

void ResourceManager::unloadTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        UnloadTexture(it->second);
        textures.erase(it);
    }
}

Sound ResourceManager::loadSound(const std::string& name, const std::string& path) {
    // Check if already loaded
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        return it->second;
    }
    
    // Load the sound
    Sound sound = LoadSound(path.c_str());
    sounds[name] = sound;
    
    return sound;
}

Sound ResourceManager::getSound(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        return it->second;
    }
    
    // Return empty sound if not found
    Sound empty = {0};
    return empty;
}

void ResourceManager::unloadSound(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        UnloadSound(it->second);
        sounds.erase(it);
    }
}

Music ResourceManager::loadMusic(const std::string& name, const std::string& path) {
    // Check if already loaded
    auto it = music.find(name);
    if (it != music.end()) {
        return it->second;
    }
    
    // Load the music
    Music mus = LoadMusicStream(path.c_str());
    music[name] = mus;
    
    return mus;
}

Music ResourceManager::getMusic(const std::string& name) {
    auto it = music.find(name);
    if (it != music.end()) {
        return it->second;
    }
    
    // Return empty music if not found
    Music empty = {0};
    return empty;
}

void ResourceManager::unloadMusic(const std::string& name) {
    auto it = music.find(name);
    if (it != music.end()) {
        UnloadMusicStream(it->second);
        music.erase(it);
    }
}

Font ResourceManager::loadFont(const std::string& name, const std::string& path) {
    // Check if already loaded
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return it->second;
    }
    
    // Load the font
    Font font = LoadFont(path.c_str());
    fonts[name] = font;
    
    return font;
}

Font ResourceManager::getFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return it->second;
    }
    
    // Return default font if not found
    return GetFontDefault();
}

void ResourceManager::unloadFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        UnloadFont(it->second);
        fonts.erase(it);
    }
}

void ResourceManager::unloadAll() {
    // Unload all textures
    for (auto& pair : textures) {
        UnloadTexture(pair.second);
    }
    textures.clear();
    
    // Unload all sounds
    for (auto& pair : sounds) {
        UnloadSound(pair.second);
    }
    sounds.clear();
    
    // Unload all music
    for (auto& pair : music) {
        UnloadMusicStream(pair.second);
    }
    music.clear();
    
    // Unload all fonts
    for (auto& pair : fonts) {
        UnloadFont(pair.second);
    }
    fonts.clear();
}