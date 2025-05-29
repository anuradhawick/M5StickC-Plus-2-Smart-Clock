#include "data.h"

static const char *TAG = "STORAGE";
String days[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Storage::Storage()
{
}

void Storage::init()
{
    if (prefs.begin("settings", false))
    {
        ESP_LOGD(TAG, "Preferences initialized");
    }
    else
    {
        ESP_LOGD(TAG, "Preferences initialization failed");
    }
}

Storage::~Storage()
{
    prefs.end();
    ESP_LOGD(TAG, "Preferences deinitialized");
}

void Storage::set_brightness(uint8_t brightness)
{
    prefs.putUChar("brightness", brightness);
    if (M5.Display.getBrightness() != map(brightness, 10, 100, 30, 255))
    {
        M5.Display.setBrightness(map(brightness, 10, 100, 30, 255));
    }
    ESP_LOGD(TAG, "Brightness saved to preferences: %d", brightness);
}

uint8_t Storage::get_brightness()
{
    uint8_t brightness = prefs.getUChar("brightness");
    ESP_LOGD(TAG, "Brightness read from preferences: %d", brightness);
    if (brightness < 10 || brightness > 100)
    {
        prefs.putUChar("brightness", 50);
        return 50;
    }
    else
    {
        return brightness;
    }
}

void Storage::set_wifi_ssid(String ssid)
{
    prefs.putString("wifi_ssid", ssid);
}

String Storage::get_wifi_ssid()
{
    String ssid = prefs.getString("wifi_ssid");
    ESP_LOGD(TAG, "SSID read from preferences: %s", ssid.c_str());
    return ssid;
}

void Storage::set_wifi_password(String password)
{
    prefs.putString("wifi_password", password);
}

String Storage::get_wifi_password()
{
    String password = prefs.getString("wifi_password");
    ESP_LOGD(TAG, "Password read from preferences: %s", password.c_str());
    return password;
}

bool Storage::get_beep()
{
    bool beep = prefs.getBool("beep");
    ESP_LOGD(TAG, "Beep read from preferences: %d", beep);
    return beep;
}

void Storage::set_beep(bool beep)
{
    if (beep)
    {
        StickCP2.Speaker.tone(10000, 100);
    }
    prefs.putBool("beep", beep);
    ESP_LOGD(TAG, "Beep saved to preferences: %d", beep);
}

bool Storage::get_imu()
{
    bool imu_active = prefs.getBool("imu");
    ESP_LOGD(TAG, "IMU read from preferences: %d", imu_active);
    return imu_active;
}

void Storage::set_imu(bool imu_active)
{
    prefs.putBool("imu", imu_active);
    ESP_LOGD(TAG, "IMU saved to preferences: %d", imu_active);
}

unsigned long Storage::get_sleep_delay()
{
    unsigned long sleep_delay = prefs.getULong("sleep_delay", 5000);
    ESP_LOGD(TAG, "Sleep delay read from preferences: %lu", sleep_delay);
    return sleep_delay;
}

void Storage::set_sleep_delay(unsigned long delay)
{
    prefs.putULong("sleep_delay", delay);
    ESP_LOGD(TAG, "Sleep delay saved to preferences: %lu", delay);
}

unsigned long Storage::get_dim_delay()
{
    unsigned long dim_delay = prefs.getULong("dim_delay", 5000);
    ESP_LOGD(TAG, "Dim delay read from preferences: %lu", dim_delay);
    return dim_delay;
}

void Storage::set_dim_delay(unsigned long delay)
{
    prefs.putULong("dim_delay", delay);
    ESP_LOGD(TAG, "Dim delay saved to preferences: %lu", delay);
}

long Storage::get_gmt_offset_sec()
{
    long gmt_offset = prefs.getULong("gmt_offset", 0);
    ESP_LOGD(TAG, "GMT offset read from preferences: %ld", gmt_offset);
    return gmt_offset;
}

void Storage::set_gmt_offset_sec(long offset)
{
    prefs.putULong("gmt_offset", offset);
    ESP_LOGD(TAG, "GMT offset saved to preferences: %ld", offset);
}

int Storage::get_daylight_offset_sec()
{
    int daylight_offset = prefs.getInt("daylight_offset", 0);
    ESP_LOGD(TAG, "Daylight offset read from preferences: %d", daylight_offset);
    return daylight_offset;
}

void Storage::set_daylight_offset_sec(int offset)
{
    prefs.putInt("daylight_offset", offset);
    ESP_LOGD(TAG, "Daylight offset saved to preferences: %d", offset);
}

JsonDocument Storage::get_json()
{
    JsonDocument doc;
    doc["brightness"] = get_brightness();
    doc["wifi_ssid"] = get_wifi_ssid();
    doc["wifi_password"] = get_wifi_password();
    doc["beep"] = get_beep();
    doc["imu"] = get_imu();
    doc["sleep_delay"] = get_sleep_delay();
    doc["dim_delay"] = get_dim_delay();
    doc["gmt_offset"] = get_gmt_offset_sec();
    doc["daylight_offset"] = get_daylight_offset_sec();
    ESP_LOGI(TAG, "JSON document created with current settings %s", doc.as<String>().c_str());
    return doc;
}

void Storage::set_json(JsonDocument doc)
{
    set_brightness(doc["brightness"]);
    set_wifi_ssid(doc["wifi_ssid"]);
    set_wifi_password(doc["wifi_password"]);
    set_beep(doc["beep"]);
    set_imu(doc["imu"]);
    set_sleep_delay(doc["sleep_delay"]);
    set_dim_delay(doc["dim_delay"]);
    set_gmt_offset_sec(doc["gmt_offset"]);
    set_daylight_offset_sec(doc["daylight_offset"]);
    ESP_LOGI(TAG, "Settings updated from JSON document: %s", doc.as<String>().c_str());
}

Storage storage;