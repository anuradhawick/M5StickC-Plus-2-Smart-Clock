#include <M5StickCPlus2.h>
#include <WiFi.h>
#include "time.h"

bool connect_to_wifi(String wifi_ssid, String wifi_pass);
bool disconnect_wifi();
bool update_time_date();
int8_t get_batt_percentage();
