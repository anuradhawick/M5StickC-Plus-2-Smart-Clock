#include "tasks.h"

static const char *TAG = "TASKS";

// Dimmer class
Dimmer::Dimmer()
{
    status = 0;
    init_time = millis();
}

bool Dimmer::tick()
{
    // ESP_LOGD(TAG, "Tick %d", status);
    if (status == 0 && init_time + storage.get_dim_delay() < millis())
    {
        // Go dim
        status = 1;
        go_dim();
        init_time = millis();
    }
    else if (status == 1 && init_time + storage.get_sleep_delay() < millis())
    {
        // Go dark
        status = 2;
        go_dark();
    }
    return true;
}

void Dimmer::go_dim()
{
    ESP_LOGD(TAG, "Go dim");
    StickCP2.Display.setBrightness(10);
}

void Dimmer::go_dark()
{
    ESP_LOGD(TAG, "Go dark %d", status);
    StickCP2.Display.sleep();
}

bool Dimmer::interrup()
{
    ESP_LOGD(TAG, "Interrup");
    if (status == 0)
    {
        init_time = millis();
        return false;
    }
    // turn screen on
    if (status == 2)
    {
        StickCP2.Display.wakeup();
    }

    StickCP2.Display.setBrightness(storage.get_brightness());
    init_time = millis();
    status = 0;

    return true;
}

bool Dimmer::is_dim()
{
    return status != 0;
}

// IMUManager class
IMUManager::IMUManager()
{
    StickCP2.Imu.getAccelData(&aX, &aY, &aZ);
    StickCP2.Imu.getAccelData(&next_aX, &next_aY, &next_aZ);
}

bool IMUManager::is_moved()
{
    StickCP2.Imu.getAccelData(&next_aX, &next_aY, &next_aZ);

    if (abs(next_aX - aX) > 0.5 || abs(next_aY - aY) > 0.5 || abs(next_aZ - aZ) > 0.5)
    {
        aX = next_aX;
        aY = next_aY;
        aZ = next_aZ;
        return true;
    }
    return false;
}

Dimmer dimmer;
IMUManager imumgr;
