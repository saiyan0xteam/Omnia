#pragma once
#include "../shared.h"

class IWorld {
public:
    static constexpr int VOLUME_SIZE = 32;
    static constexpr int TILE_SIZE = 16;
    int renderDistance = 2;
    std::unordered_set<int64_t> visibleVolumes;

    struct Volume {
        int cx = 0;
        int cy = 0;

        void Draw(int tileSize) {
            for (int y = 0; y < VOLUME_SIZE; y++) {
                for (int x = 0; x < VOLUME_SIZE; x++) {

                    float worldX = (float)((cx * VOLUME_SIZE + x) * tileSize);
                    float worldY = (float)((cy * VOLUME_SIZE + y) * tileSize);

                    bool checker = ((x + y) % 2 == 0);

                    Color grass = checker
                        ? Color{ 80, 100, 60, 255 }
                    : Color{ 75, 80, 40, 255 };

                    DrawRectangle(
                        (int)worldX,
                        (int)worldY,
                        tileSize,
                        tileSize,
                        grass
                    );
                }
            }
        }
    };

private:
    std::unordered_map<int64_t, Volume> Volumes;

    int64_t Key(int cx, int cy) const {
        return (int64_t(cx) << 32) | (uint32_t)cy;
    }

public:
    Volume& GetVolume(int cx, int cy) {
        int64_t key = Key(cx, cy);

        auto it = Volumes.find(key);
        if (it == Volumes.end()) {
            Volume c;
            c.cx = cx;
            c.cy = cy;
            Volumes[key] = c;
        }

        return Volumes[key];
    }

    void Fire(Vector2 playerPos) {

        visibleVolumes.clear();

        int cx = (int)(playerPos.x / (VOLUME_SIZE * TILE_SIZE));
        int cy = (int)(playerPos.y / (VOLUME_SIZE * TILE_SIZE));

        for (int y = -renderDistance; y <= renderDistance; y++) {
            for (int x = -renderDistance; x <= renderDistance; x++) {

                int nx = cx + x;
                int ny = cy + y;

                int64_t key = Key(nx, ny);

                GetVolume(nx, ny);
                visibleVolumes.insert(key);
            }
        }
    }

    void Paint() {
        for (auto key : visibleVolumes) {
            Volumes[key].Draw(TILE_SIZE);
        }
    }

    void CleanOldVolumes() {
        for (auto it = Volumes.begin(); it != Volumes.end(); ) {

            if (visibleVolumes.find(it->first) == visibleVolumes.end()) {
                it = Volumes.erase(it);
            }
            else {
                ++it;
            }
        }
    }
};