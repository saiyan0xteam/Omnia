#pragma once
#include <discord_rpc.h>
#include <discord_register.h>

class Discord {
public:
	void Initialize(const char* id);
	void Update();
};
