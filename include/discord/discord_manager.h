#ifndef DISCORD_MANAGER_H
#define DISCORD_MANAGER_H

namespace discord_sdk {
    bool initialize();
    void update();
    void shutdown();
    
    // Game-specific Discord Rich Presence functions
    void updatePresence(const char* state, const char* details);
}

#endif // DISCORD_MANAGER_H