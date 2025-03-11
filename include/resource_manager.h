#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "raylib.h"
#include <unordered_map>
#include <string>

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    // Texture management
    Texture2D loadTexture(const std::string& name, const std::string& path);
    Texture2D loadTexture(const std::string& name, const std::string& path, Texture2D texture);
    Texture2D getTexture(const std::string& name);
    void unloadTexture(const std::string& name);
    
    // Sound management
    Sound loadSound(const std::string& name, const std::string& path);
    Sound getSound(const std::string& name);
    void unloadSound(const std::string& name);
    
    // Music management
    Music loadMusic(const std::string& name, const std::string& path);
    Music getMusic(const std::string& name);
    void unloadMusic(const std::string& name);
    
    // Font management
    Font loadFont(const std::string& name, const std::string& path);
    Font getFont(const std::string& name);
    void unloadFont(const std::string& name);
    
    // Clean up all resources
    void unloadAll();
    
private:
    ResourceManager() {} // Private constructor for singleton
    ~ResourceManager() { unloadAll(); }
    
    // Prevent copying and assignment
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Sound> sounds;
    std::unordered_map<std::string, Music> music;
    std::unordered_map<std::string, Font> fonts;
};

#endif // RESOURCE_MANAGER_H