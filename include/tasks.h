#pragma once
#include <M5StickCPlus2.h>
#include "data.h"

class Dimmer
{
public:
    Dimmer();
    bool tick();
    bool is_dim();
    bool interrup();

private:
    void go_dim();
    void go_dark();
    uint8_t brightness;
    uint8_t status; // 0 = normal, 1 = dimmed, 2 = asleep
    unsigned long init_time;
};

class IMUManager
{
public:
    IMUManager();
    bool is_moved();

private:
    float aX = 0.0F, aY = 0.0F, aZ = 0.0F, next_aX = 0.0F, next_aY = 0.0F, next_aZ = 0.0F;
};

extern Dimmer dimmer;
extern IMUManager imumgr;