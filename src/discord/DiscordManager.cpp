#include "discord/DiscordManager.hpp"
#include "discord.h"
#include <ctime>

std::unique_ptr<discord::Core> DiscordManager::s_core;

bool DiscordManager::Initialize() {
    discord::ClientId clientId = 1342619188707197019;
    discord::Core* rawCore = nullptr;
    auto result = discord::Core::Create(clientId, DiscordCreateFlags_NoRequireDiscord, &rawCore);

    if (result != discord::Result::Ok) {
        return false;
    }

    s_core.reset(rawCore);

    discord::Activity activity{};
    activity.SetType(discord::ActivityType::Playing);
    activity.GetTimestamps().SetStart(std::time(nullptr));
    activity.GetAssets().SetLargeImage("icon");

    s_core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});

    return true;
}

void DiscordManager::Update() {
    if (s_core) {
        s_core->RunCallbacks();
    }
}

void DiscordManager::UpdatePresence(const char* state, const char* details) {
    if (!s_core) return;
    
    discord::Activity activity{};
    activity.SetType(discord::ActivityType::Playing);
    activity.SetState(state);
    activity.SetDetails(details);
    activity.GetTimestamps().SetStart(std::time(nullptr));
    activity.GetAssets().SetLargeImage("icon");
    
    s_core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
}

void DiscordManager::Shutdown() {
    s_core.reset();
}