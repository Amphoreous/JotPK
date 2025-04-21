#pragma once
#include <memory>

namespace discord {
    class Core;
}

class DiscordManager {
public:
    static bool Initialize();
    static void Update();
    static void Shutdown();
    static void UpdatePresence(const char* state, const char* details);

private:
    static std::unique_ptr<discord::Core> s_core;
};