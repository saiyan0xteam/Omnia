#pragma once
#include "../shared.h"
#include "ITexture.h"

class IPlayer {
public:
	enum Direction {
		DOWN,
		UP,
		LEFT,
		RIGHT
	};

	Direction dir = DOWN;
	bool isMoving = false;
	float speed = 200.0f;
	Vector2 pos;
	ITexture idle_down;
	ITexture idle_up;
	ITexture idle_left;
	ITexture idle_right;
	ITexture walk_down;
	ITexture walk_up;
	ITexture walk_left;
	ITexture walk_right;
	ITexture* currentAnim = nullptr;

	void Power() {
		Logger::Success("Interfaces", "IPlayer Successfully Powered!");
		idle_down.Load((WorkingDirectory + "\\textures\\character\\adelina_idle_down1.png").c_str(), (WorkingDirectory + "\\textures\\character\\adelina_idle_down2.png").c_str());
		idle_up.Load((WorkingDirectory + "\\textures\\character\\adelina_idle_up1.png").c_str(), (WorkingDirectory + "\\textures\\character\\adelina_idle_up2.png").c_str());
		idle_left.Load((WorkingDirectory + "\\textures\\character\\adelina_idle_left1.png").c_str(), (WorkingDirectory + "\\textures\\character\\adelina_idle_left2.png").c_str());
		idle_right.Load((WorkingDirectory + "\\textures\\character\\adelina_idle_right1.png").c_str(), (WorkingDirectory + "\\textures\\character\\adelina_idle_right2.png").c_str());
		walk_down.Load((WorkingDirectory + "\\textures\\character\\adelina_walk_down1.png").c_str(), (WorkingDirectory + "\\textures\\character\\adelina_walk_down2.png").c_str());
		walk_up.Load((WorkingDirectory + "\\textures\\character\\adelina_walk_up1.png").c_str(), (WorkingDirectory + "\\textures\\character\\adelina_walk_up2.png").c_str());
		walk_left.Load((WorkingDirectory + "\\textures\\character\\adelina_walk_left1.png").c_str(), (WorkingDirectory + "\\textures\\character\\adelina_walk_left2.png").c_str());
		walk_right.Load((WorkingDirectory + "\\textures\\character\\adelina_walk_right1.png").c_str(), (WorkingDirectory + "\\textures\\character\\adelina_walk_right2.png").c_str());
		pos = { 0, 0 };
	}

	void Fire(float dt) {
		isMoving = false;

		if (IsKeyDown(KEY_D)) {
			pos.x += speed * dt;
			dir = RIGHT;
			isMoving = true;
		}
		else if (IsKeyDown(KEY_A)) {
			pos.x -= speed * dt;
			dir = LEFT;
			isMoving = true;
		}
		else if (IsKeyDown(KEY_W)) {
			pos.y -= speed * dt;
			dir = UP;
			isMoving = true;
		}
		else if (IsKeyDown(KEY_S)) {
			pos.y += speed * dt;
			dir = DOWN;
			isMoving = true;
		}

		if (isMoving) {
			switch (dir) {
			case DOWN:  currentAnim = &walk_down; break;
			case UP:    currentAnim = &walk_up; break;
			case LEFT:  currentAnim = &walk_left; break;
			case RIGHT: currentAnim = &walk_right; break;
			}
		}
		else {
			switch (dir) {
			case DOWN:  currentAnim = &idle_down; break;
			case UP:    currentAnim = &idle_up; break;
			case LEFT:  currentAnim = &idle_left; break;
			case RIGHT: currentAnim = &idle_right; break;
			}
		}

		currentAnim->AnimSwap(dt);
		DrawTexture(currentAnim->GetTexture(), pos.x, pos.y, WHITE);
	}

	void Destroy() {
		UnloadTexture(idle_down.tex[0]);
		UnloadTexture(idle_down.tex[1]);
		UnloadTexture(idle_up.tex[0]);
		UnloadTexture(idle_up.tex[1]);
		UnloadTexture(idle_left.tex[0]);
		UnloadTexture(idle_left.tex[1]);
		UnloadTexture(idle_right.tex[0]);
		UnloadTexture(idle_right.tex[1]);
		UnloadTexture(walk_down.tex[0]);
		UnloadTexture(walk_down.tex[1]);
		UnloadTexture(walk_up.tex[0]);
		UnloadTexture(walk_up.tex[1]);
		UnloadTexture(walk_left.tex[0]);
		UnloadTexture(walk_left.tex[1]);
		UnloadTexture(walk_right.tex[0]);
		UnloadTexture(walk_right.tex[1]);
		Logger::Success("Interfaces", "IPlayer Successfully Destroyed!");
	}
};