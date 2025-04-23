#include "util.h"

// WIFI Creds
static const char *TAG = "WIFI";

// Time
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 9 * 3600 + 1800; // ACST = UTC+9:30 (34200 sec)
const int daylightOffset_sec = 0;

bool connect_to_wifi(String wifi_ssid, String wifi_pass)
{
    ESP_LOGI(TAG, "Connecting to WiFi...\n\tSSID: %s\n\tPassword: %s", wifi_ssid, wifi_pass);

    // try to connect to WiFi
    M5.begin();
    WiFi.begin(wifi_ssid, wifi_pass);
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        counter++;
        if (counter > 20)
        {
            return false;
        }
    }
    return true;
}

bool disconnect_wifi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return true;
    }
    else
    {
        WiFi.mode(WIFI_OFF);
    }
    return false;
}

bool update_time_date()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
    {
        return false;
    }
    // Convert and set RTC time
    m5::rtc_time_t timeStruct;
    m5::rtc_date_t dateStruct;

    timeStruct.hours = timeinfo.tm_hour;
    timeStruct.minutes = timeinfo.tm_min;
    timeStruct.seconds = timeinfo.tm_sec;

    dateStruct.weekDay = timeinfo.tm_wday;
    dateStruct.month = timeinfo.tm_mon + 1; // tm_mon is 0-11
    dateStruct.date = timeinfo.tm_mday;
    dateStruct.year = timeinfo.tm_year + 1900; // tm_year is years since 1900

    StickCP2.Rtc.setTime(&timeStruct);
    StickCP2.Rtc.setDate(&dateStruct);

    ESP_LOGI(TAG, "Year: %d Month: %d Day: %d", dateStruct.year, dateStruct.month, dateStruct.date);

    return true;
}

int8_t get_batt_percentage()
{
    float volt = StickCP2.Power.getBatteryVoltage();
    int percentage = map(volt, 3700, 4200, 0, 100);

    if (percentage < 0)
    {
        percentage = 0;
    }
    else if (percentage > 100)
    {
        percentage = 100;
    }
    return percentage;
}
