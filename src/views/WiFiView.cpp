#include "views/WiFiView.h"

static const char *TAG = "WIFI_VIEW";

WiFiView::WiFiView()
{
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(true);

    disp_buffer = new M5Canvas(&M5.Lcd);

    disp_buffer->setSwapBytes(true);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);
    disp_buffer->pushSprite(0, 0);

    disp_buffer->setTextSize(1);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);

    loader_last_rendered = 0;
    text_last_rendered = 0;
    loader_frame = 0;

    inited_time = millis();

    render_text();
    render_loader();
}

WiFiView::~WiFiView()
{
    ESP_LOGD(TAG, "Destructor called");
    disconnect_wifi();

    disp_buffer->deleteSprite();
    delete disp_buffer;

    ESP_LOGD(TAG, "Destructor finished");
}

void WiFiView::render_loader()
{
    disp_buffer->pushImage(240 - loader_ani_width,
                           67 - loader_ani_height / 2,
                           loader_ani_width,
                           loader_ani_height,
                           loader_ani_imgs[loader_frame]);
    loader_frame++;

    if (loader_frame == loader_ani_frames)
    {
        loader_frame = 0;
    }
    disp_buffer->pushSprite(0, 0);
}

void WiFiView::render_text()
{
    switch (wifi_started)
    {
    case 0:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("WIFI", 10, 10, &fonts::Font4);
        disp_buffer->drawString("Linking", 10, 30, &fonts::Font4);
        disp_buffer->drawString("Wait " + String(5 - (millis() - inited_time) / 1000) + "s", 10, 70, &fonts::Font2);
        disp_buffer->drawString("to Continue", 10, 90, &fonts::Font2);
        disp_buffer->pushSprite(0, 0);
        break;
    case 1:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("WIFI", 10, 10, &fonts::Font4);
        disp_buffer->drawString("Linked", 10, 30, &fonts::Font4);
        disp_buffer->drawString("IP Address", 10, 70, &fonts::Font2);
        disp_buffer->drawString(ip_address, 10, 90, &fonts::Font2);
        disp_buffer->drawString("Updating time", 10, 110, &fonts::Font2);
        disp_buffer->pushSprite(0, 0);
        break;
    case 2:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("WIFI", 10, 10, &fonts::Font4);
        disp_buffer->drawString("Failed", 10, 30, &fonts::Font4);
        disp_buffer->drawString("Check credentials", 10, 70, &fonts::Font2);
        disp_buffer->drawString("Reset SSID and", 10, 90, &fonts::Font2);
        disp_buffer->drawString("Password Again", 10, 110, &fonts::Font2);
        disp_buffer->pushSprite(0, 0);
        break;
    case 3:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("WIFI", 10, 10, &fonts::Font4);
        disp_buffer->drawString("Linked", 10, 30, &fonts::Font4);
        disp_buffer->drawString("Time Updated", 10, 70, &fonts::Font2);
        disp_buffer->pushSprite(0, 0);
        break;
    case 4:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("WIFI", 10, 10, &fonts::Font4);
        disp_buffer->drawString("Linked", 10, 30, &fonts::Font4);
        disp_buffer->drawString("Failed to Update", 10, 70, &fonts::Font2);
        disp_buffer->drawString("Time", 10, 90, &fonts::Font2);
        disp_buffer->pushSprite(0, 0);
    default:
        break;
    }
}

void WiFiView::render()
{
    if (millis() > loader_last_rendered + 100)
    {
        loader_last_rendered = millis();
        render_loader();
    }

    if (millis() > text_last_rendered + 1000)
    {
        text_last_rendered = millis();
        render_text();
    }

    else if (!wifi_started && inited_time + 5000 < millis())
    {
        if (connect_to_wifi(storage.get_wifi_ssid(), storage.get_wifi_password()))
        {
            ip_address = WiFi.localIP().toString();
            wifi_started = 1;
            inited_time = millis();
        }
        else
        {
            ESP_LOGE(TAG, "Failed to connect to WiFi");
            wifi_started = 2;
        }
    }

    else if (wifi_started == 1 && inited_time + 2000 < millis())
    {
        if (update_time_date(storage.get_gmt_offset_sec(), storage.get_daylight_offset_sec()))
        {
            wifi_started = 3;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to update time");
            wifi_started = 4;
        }
    }
}
