#include "discord_presence.h"
#include "discord_game_sdk.h"
#include <time.h>
#include <stdio.h>

struct IDiscordCore* core;
struct IDiscordActivityManager* activityManager;

void initDiscord() {
    struct DiscordCreateParams params;
    DiscordCreateParamsSetDefault(&params);
    params.client_id = 1342619188707197019;
    params.flags = DiscordCreateFlags_NoRequireDiscord;  // Permitir que el juego se ejecute sin Discord
    enum EDiscordResult result = DiscordCreate(DISCORD_VERSION, &params, &core);
    if (result != DiscordResult_Ok) {
        printf("Error al inicializar Discord: %d\n", result);
        return;
    }

    activityManager = core->get_activity_manager(core);

    struct DiscordActivity activity;
    memset(&activity, 0, sizeof(activity));
    activity.type = DiscordActivityType_Playing;
    activity.timestamps.start = time(NULL);
    strcpy(activity.assets.large_image, "icon");

    activityManager->update_activity(activityManager, &activity, NULL, NULL);

    // Llamar a updateDiscord inmediatamente después de inicializar Discord
    updateDiscord();
}

void updateDiscord() {
    if (core) {
        core->run_callbacks(core);
    }
}