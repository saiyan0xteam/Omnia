#pragma once
#include "../shared.h"

class CUserInterface {
public:
    virtual void Power() = 0;
    virtual void Fire(float dt) = 0;
    virtual void Paint() = 0;
    virtual void Destroy() = 0;
    virtual ~CUserInterface() {}
};
