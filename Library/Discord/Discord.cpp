#include "Discord.h"
#include <time.h>
#include <chrono>

static int64_t eptime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

void Discord::Initialize(const char* id)
{
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	Discord_Initialize(id, &Handle, 1, NULL);
}

void Discord::Update()
{
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.details = "Sandbox Mode";
	discordPresence.startTimestamp = eptime;
	discordPresence.largeImageKey = "logo";
	discordPresence.largeImageText = "Omnia";
	Discord_UpdatePresence(&discordPresence);
}