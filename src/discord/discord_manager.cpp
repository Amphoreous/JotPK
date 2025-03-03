#include "discord/discord_manager.h"
#include "discord.h"

#include <memory>
#include <ctime>

namespace {
    std::unique_ptr<discord::Core> core;
}

namespace discord_sdk {
    bool initialize() {
        discord::ClientId clientId = 1342619188707197019;
        discord::Core* rawCore = nullptr;
        auto result = discord::Core::Create(clientId, DiscordCreateFlags_NoRequireDiscord, &rawCore);

        if (result != discord::Result::Ok) {
            return false;
        }

        core.reset(rawCore);

        discord::Activity activity{};
        activity.SetType(discord::ActivityType::Playing);
        activity.GetTimestamps().SetStart(std::time(nullptr));
        activity.GetAssets().SetLargeImage("icon");

        core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});

        return true;
    }
    
    void update() {
        if (core) {
            core->RunCallbacks();
        }
    }
    
    void updatePresence(const char* state, const char* details) {
        if (!core) return;
        
        discord::Activity activity{};
        activity.SetType(discord::ActivityType::Playing);
        activity.SetState(state);
        activity.SetDetails(details);
        activity.GetTimestamps().SetStart(std::time(nullptr));
        activity.GetAssets().SetLargeImage("icon");
        
        core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
    }
    
    void shutdown() {
        core.reset();
    }
}