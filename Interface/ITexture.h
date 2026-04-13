#pragma once
#include "../shared.h"

class ITexture {
public:
	Texture2D tex[2];
	int frame = 0;
	float timer = 0.0f;
	float interval = 0.5f;

	void Load(const char* file1, const char* file2) {
		tex[0] = LoadTexture(file1);
		tex[1] = LoadTexture(file2);
	}

	void AnimSwap(float dt) {
		timer += dt;
		if (timer >= interval) {
			timer = 0.0f;
			frame = (frame + 1) % 2;
		}
	}

	Texture2D GetTexture() {
		return tex[frame];
	}
};