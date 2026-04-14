#pragma once
#include "../shared.h"

class IFPSCounter {
public:
    std::vector<float> frameTimes;
    const int maxSamples = 300;

    void Update() {
        float dt = GetFrameTime() * 1000.0f;

        frameTimes.push_back(dt);
        if (frameTimes.size() > maxSamples)
            frameTimes.erase(frameTimes.begin());
    }

    float GetFPS() {
        return 1000.0f / GetAverage();
    }

    float GetAverage() {
        float sum = 0;
        for (float t : frameTimes) sum += t;
        return frameTimes.empty() ? 0 : sum / frameTimes.size();
    }

    float GetPercentile(float p) {
        if (frameTimes.empty()) return 0;

        std::vector<float> copy = frameTimes;
        std::sort(copy.begin(), copy.end());

        int index = (int)(p * copy.size());
        return copy[index];
    }

    float GetP1() {
        return GetPercentile(0.99f); // %1 worst
    }

    float GetP01() {
        return GetPercentile(0.999f); // %0.1 worst
    }
};