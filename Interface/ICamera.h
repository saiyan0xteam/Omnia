#pragma once
#include "../shared.h"

class ICamera {
public:
	Camera2D camera = { 0 };
	
	void Power() {
		Logger::Success("Interfaces", "ICamera Successfully Powered!");
		camera.offset = { (float)sW / 2.0f, (float)sH / 2.0f };
		camera.target = { 32.0f, 32.0f };
		camera.rotation = 0.0f;
		camera.zoom = 1.0f;
	}

	void Fire(float dt, Vector2 chase) {
		Vector2 targetPos = { chase.x + 32.0f, chase.y + 32.0f };
		camera.target = Vector2Lerp(camera.target, targetPos, 0.1f);
	}

	void Enter() {
		BeginMode2D(camera);
	}

	void Destroy() {
		EndMode2D();
	}
};