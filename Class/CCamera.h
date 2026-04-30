#pragma once
#include "../shared.h"

class CCamera {
public:
	Camera2D camera = { };
	float targetZoom = 1.0f;
	
	void Power() {
		Logger::Success("Interfaces", "ICamera Successfully Powered!");
		camera.offset = { (float)sW / 2.0f, (float)sH / 2.0f };
		camera.target = { 32.0f, 32.0f };
		camera.rotation = 0.0f;
		camera.zoom = 1.0f;
	}

	void Fire(float dt, Vector2 chase) {
		camera.offset = { (float)sW / 2.0f, (float)sH / 2.0f };
		Vector2 targetPos = { chase.x + 32.0f, chase.y + 32.0f };
		camera.target = Vector2Lerp(camera.target, targetPos, 0.1f);
		if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
			float wheel = GetMouseWheelMove();
			if (wheel != 0.0f) {
				targetZoom += wheel * 0.3f;
				targetZoom = Clamp(targetZoom, 0.8f, 2.5f);
			}
		}
		camera.zoom = Lerp(camera.zoom, targetZoom, 10.0f * dt);
	}

	void Enter() {
		BeginMode2D(camera);
	}

	void Destroy() {
		EndMode2D();
	}
};