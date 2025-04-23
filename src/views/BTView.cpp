#include "views/BTView.h"

static const char *TAG = "BT_VIEW";
BluetoothSerial *BTView::btSerial = nullptr;
BTView *BTView::instance = nullptr;

void BTView::static_bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if (BTView::instance)
    {
        BTView::instance->bt_callback(event, param);
    }
}

BTView::BTView()
{
    instance = this;
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

BTView::~BTView()
{
    ESP_LOGD(TAG, "Destructor called");
    if (bt_started > 0)
    {
        BTView::btSerial->disconnect();
        BTView::btSerial->end();
        delete BTView::btSerial;
        BTView::btSerial = nullptr;
    }

    disp_buffer->deleteSprite();
    delete disp_buffer;

    ESP_LOGD(TAG, "Destructor finished");
}

bool BTView::receive_event(EVENTS::event event)
{
    if (event == EVENTS::HOME_PRESSED)
    {
        if (bt_started == 2)
        {
            write_to_bt("Please disconnect connection.\n");
            return true;
        }
        return false;
    }
    return false;
}

String BTView::get_main_text()
{
    String text = "";
    text += "Send the option number..\
    \n1. WiFi options\
    \n2. Set time\
    \n3. Set date\
    \n4: Toggle beep (";
    text += storage.get_beep()
                ? "ON"
                : "OFF";
    text += ")\n5: Toogle dimming (";
    text += storage.get_imu()
                ? "ON"
                : "OFF";
    text += ")\n6: Set dimming delay (";
    text += storage.get_dim_delay() / 1000;
    text += ")\n7: Set sleep delay (";
    text += storage.get_sleep_delay() / 1000;
    text += ")\n8: About\n";
    return text;
}

void BTView::write_to_bt(const char *data)
{
    if (bt_started > 0)
    {
        btSerial->write((uint8_t *)data, strlen(data));
    }
}

void BTView::render_loader()
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

void BTView::render_text()
{
    switch (bt_started)
    {
    case 0:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("BT Loading", 10, 10, &fonts::Font4);
        disp_buffer->drawString("Wait " + String(5 - (millis() - inited_time) / 1000) + "s", 10, 50, &fonts::Font2);
        disp_buffer->drawString("to Continue", 10, 70, &fonts::Font2);
        disp_buffer->pushSprite(0, 0);
        break;
    case 1:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("BT Active", 10, 10, &fonts::Font4);
        disp_buffer->drawString("Connect with your", 10, 50, &fonts::Font2);
        disp_buffer->drawString("BT Serial App", 10, 70, &fonts::Font2);
        disp_buffer->drawString("to Continue", 10, 90, &fonts::Font2);
        disp_buffer->pushSprite(0, 0);
        break;
    case 2:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("BT Linked", 10, 10, &fonts::Font4);
        disp_buffer->drawString("Device Connected", 10, 50, &fonts::Font2);
        disp_buffer->drawString("Follow the Terminal", 10, 70, &fonts::Font2);
        disp_buffer->drawString("Instructions", 10, 90, &fonts::Font2);
        disp_buffer->pushSprite(0, 0);
        break;
    }
}

void BTView::render()
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

    if (bt_started > 0)
    {
        bt_loop();
    }
    else if (!bt_started && inited_time + 5000 < millis())
    {
        BTView::btSerial = new BluetoothSerial();
        BTView::btSerial->register_callback(static_bt_callback);
        BTView::btSerial->begin("M5Stick");
        ESP_LOGD(TAG, "BT Serial Started");
        bt_started = 1;
    }
}

void BTView::bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if (event == ESP_SPP_SRV_OPEN_EVT)
    {
        ESP_LOGD(TAG, "Client Connected");
        write_to_bt(get_main_text().c_str());
        main_menu = 0;
        bt_started = 2;
    }

    if (event == ESP_SPP_CLOSE_EVT)
    {
        ESP_LOGD(TAG, "Client disconnected");
        main_menu = 0;
        bt_started = 1;
    }
}

void BTView::bt_loop()
{
    if (!BTView::btSerial->available())
    {
        return;
    }

    bt_incoming = BTView::btSerial->readString();
    bt_incoming.trim();
    ESP_LOGD(TAG, "Incoming from BT Serial - %s", bt_incoming.c_str());

    switch (main_menu)
    {
    case 0:
        // just reusing the variable
        main_menu = (uint8_t)bt_incoming.toInt();
        text = "";

        switch (main_menu)
        {
        case 1:
            text = "Enter WIFI SSID\n";
            break;
        case 2:
            text = "Enter time as HH:MM\n";
            break;
        case 3:
            text = "Enter date as DD/MM/YYYY/D, D=1 for Sunday\n";
            break;
        case 4:
            storage.set_beep(!storage.get_beep());
            if (storage.get_beep())
            {
                text = "Beep enabled\n";
                write_to_bt(text.c_str());
                text = get_main_text();
            }
            else
            {
                text = "Beep disabled\n";
                write_to_bt(text.c_str());
                text = get_main_text();
            }
            main_menu = 0;
            break;
        case 5:
            storage.set_imu(!storage.get_imu());
            if (storage.get_imu())
            {
                text = "Auto dimming enabled\n";
                write_to_bt(text.c_str());
                text = get_main_text();
            }
            else
            {
                text = "Auto dimming disabled\n";
                write_to_bt(text.c_str());
                text = get_main_text();
            }
            main_menu = 0;
            break;
        case 6:
            text = "Enter dimming delay in seconds\n";
            break;
        case 7:
            text = "Enter sleep delay in seconds\n";
            break;
        case 8:
            text = "Created by:\n Anuradha Wickramarachchi\n anuradhawick.com\n hello@anuradhawick.com\n";
            break;
        default:
            break;
        }
        break;
    case 1:
        ESP_LOGD(TAG, "Update ssid or password");
        if (ssid.length() == 0)
        {
            ssid = bt_incoming.c_str();
            text = "SSID successfully updated\nNow enter the WIFI password\n";
            ESP_LOGD(TAG, "Update ssid to \"%s\"", ssid.c_str());
        }
        else
        {
            password = bt_incoming.c_str();
            text = "Password successfully updated\nProceed to WiFi screen to confirm functionality\n";
            write_to_bt(text.c_str());
            ESP_LOGD(TAG, "Update password to \"%s\"", password.c_str());
            storage.set_wifi_ssid(ssid);
            storage.set_wifi_password(password);
            ssid = "";
            password = "";
            main_menu = 0;
            text = get_main_text();
        }
        break;
    case 2:
        RTC_TimeStruct.hours = (uint8_t)bt_incoming.substring(0, 2).toInt();
        RTC_TimeStruct.minutes = (uint8_t)bt_incoming.substring(3, 5).toInt();

        ESP_LOGD(TAG, "Update time to %d:%d",
                 RTC_TimeStruct.hours,
                 RTC_TimeStruct.minutes,
                 bt_incoming.substring(0, 2),
                 bt_incoming.substring(3, 5));

        M5.Rtc.setTime(&RTC_TimeStruct);

        text = "Time successfully updated\n";
        write_to_bt(text.c_str());
        text = get_main_text();
        break;
    case 3:
        RTC_DateStruct.date = (uint8_t)bt_incoming.substring(0, 2).toInt();
        RTC_DateStruct.month = (uint8_t)bt_incoming.substring(3, 5).toInt();
        RTC_DateStruct.year = (uint16_t)bt_incoming.substring(6, 10).toInt();
        RTC_DateStruct.weekDay = (uint16_t)bt_incoming.substring(11, 12).toInt() - 1;

        ESP_LOGD(TAG, "Update date to %d/%d/%d/%d",
                 RTC_DateStruct.date,
                 RTC_DateStruct.month,
                 RTC_DateStruct.year,
                 RTC_DateStruct.weekDay);

        M5.Rtc.setDate(&RTC_DateStruct);

        text = "Date successfully updated\n";
        write_to_bt(text.c_str());
        text = get_main_text();
        break;
    case 6:
    {
        long dim_delay = bt_incoming.toInt();
        dim_delay = max(min(dim_delay, (long)60), (long)5);
        storage.set_dim_delay(dim_delay * 1000);
        text = "Dimming delay successfully updated to: ";
        text += dim_delay;
        text += "s\n";
        main_menu = 0;
        write_to_bt(text.c_str());
        text = get_main_text();
        break;
    }
    case 7:
    {
        long sleep_delay = bt_incoming.toInt();
        sleep_delay = max(min(sleep_delay, (long)60), (long)5);
        storage.set_sleep_delay(sleep_delay * 1000);
        text = "Sleep delay successfully updated to: ";
        text += sleep_delay;
        text += "s\n";
        main_menu = 0;
        write_to_bt(text.c_str());
        text = get_main_text();
        break;
    }
    default:
        text = "Invalid option\n";
        break;
    }

    write_to_bt(text.c_str());
}