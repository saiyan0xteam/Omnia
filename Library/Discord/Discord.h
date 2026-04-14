#pragma once
#include "DiscordSDK/include/discord_register.h"
#include "DiscordSDK/include/discord_rpc.h"
#include <Windows.h>

class Discord {
public:
	void Initialize(const char* id);
	void Update();
};
