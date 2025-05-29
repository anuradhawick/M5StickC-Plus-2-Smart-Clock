#pragma once

#include <Preferences.h>
#include <M5StickCPlus2.h>
#include <ArduinoJson.h>

class Storage
{
public:
    Storage();
    ~Storage();

    void init();
    void set_brightness(uint8_t brightness);
    uint8_t get_brightness();

    void set_wifi_ssid(String ssid);
    String get_wifi_ssid();

    void set_wifi_password(String password);
    String get_wifi_password();

    bool get_beep();
    void set_beep(bool beep);

    bool get_imu();
    void set_imu(bool imu);

    unsigned long get_sleep_delay();
    void set_sleep_delay(unsigned long delay);

    unsigned long get_dim_delay();
    void set_dim_delay(unsigned long delay);

    long get_gmt_offset_sec();
    void set_gmt_offset_sec(long offset);
    
    int get_daylight_offset_sec();
    void set_daylight_offset_sec(int offset);

    JsonDocument get_json();
    void set_json(JsonDocument doc);

private:
    bool started = false;
    Preferences prefs;
};

extern Storage storage;