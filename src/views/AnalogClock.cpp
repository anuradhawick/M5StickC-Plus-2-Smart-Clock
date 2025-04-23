#include <M5StickCPlus2.h>
#include <M5GFX.h>

#include "views/View.h"
#include "views/AnalogClock.h"
#include "data.h"

static const char *TAG = "AN_CLOCK";

m5::rtc_time_t RTC_TimeStruct;
m5::rtc_date_t RTC_DateStruct;
extern String days[7];

AnalogClock::AnalogClock()
{
    StickCP2.begin();
    StickCP2.Lcd.setRotation(3);
    StickCP2.Lcd.setSwapBytes(false);

    disp_buffer = new M5Canvas(&StickCP2.Display);
    disp_buffer->setSwapBytes(false);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);
    disp_buffer->pushSprite(0, 0);

    last_render = 0;
    origin_x = StickCP2.Lcd.height() / 2 + 5;
    origin_y = StickCP2.Lcd.height() / 2;

    StickCP2.Rtc.getTime(&RTC_TimeStruct);
    StickCP2.Rtc.getDate(&RTC_DateStruct);

    render_batt_power();
    render_clock_face();
    render_date();
}

AnalogClock::~AnalogClock()
{
    ESP_LOGD(TAG, "Destructor called");
    disp_buffer->deleteSprite();
    delete disp_buffer;
    ESP_LOGD(TAG, "Destructor finished");
}

void AnalogClock::render_date()
{
    dt_cache = RTC_DateStruct.date;
    mn_cache = RTC_DateStruct.month;
    yr_cache = RTC_DateStruct.year;

    sprintf(dateDDMMYYYY, "%02d/%02d/%04d", dt_cache, mn_cache, yr_cache);

    disp_buffer->fillRect(140, 50, 100, 85, BLACK);
    disp_buffer->drawString(dateDDMMYYYY,
                            140 + (100 - StickCP2.Lcd.textWidth("10/01/2000")) / 2 - 5,
                            67 - disp_buffer->fontHeight(&fonts::Font2),
                            &fonts::Font2);
    disp_buffer->drawString(dateDDMMYYYY,
                            140 + (100 - StickCP2.Lcd.textWidth("10/01/2000")) / 2 - 5,
                            67 - disp_buffer->fontHeight(&fonts::Font2),
                            &fonts::Font2);
    d_cache = RTC_DateStruct.weekDay;

    disp_buffer->drawString(days[d_cache].c_str(),
                            140 + (100 - StickCP2.Lcd.textWidth(days[d_cache].c_str())) / 2,
                            67,
                            &fonts::Font2);

    disp_buffer->drawFastHLine(140, 67, 100, WHITE);

    disp_buffer->pushSprite(0, 0);
}

void AnalogClock::render_batt_power()
{
    bat_cache = get_batt_percentage();
    bat_power = String(bat_cache) + "%";
    ESP_LOGD(TAG, "Batt: %s", bat_power);

    disp_buffer->fillRect(140, 0, 100, 50, BLACK);
    disp_buffer->drawString(bat_power,
                            240 - StickCP2.Display.textWidth(bat_power, &fonts::Font2) - 10,
                            5,
                            &fonts::Font2);
}

void AnalogClock::render_clock_face()
{
    s_cache = RTC_TimeStruct.seconds;
    m_cache = RTC_TimeStruct.minutes;
    h_cache = RTC_TimeStruct.hours;

    disp_buffer->fillRect(0, 0, 140, 135, BLACK);

    for (int32_t i = 0; i < 60; i++)
    {
        disp_buffer->fillCircle(origin_x + round((seconds_hand_length + 3) * cos(i * 6 * PI / 180)),
                                origin_y - round((seconds_hand_length + 3) * sin(i * 6 * PI / 180)),
                                3 ? i % 5 == 0 : 1,
                                WHITE);
    }

    seconds_angle = s_cache * 6;
    minutes_angle = m_cache * 6;
    hours_angle = h_cache * 5 * 6 + m_cache * 0.5;

    disp_buffer->fillTriangle(origin_x - round(hours_hand_width * cos(hours_angle * PI / 180)),
                              origin_y - round(hours_hand_width * sin(hours_angle * PI / 180)),
                              origin_x + round(hours_hand_width * cos(hours_angle * PI / 180)),
                              origin_y + round(hours_hand_width * sin(hours_angle * PI / 180)),
                              origin_x + round(hours_hand_length * sin(hours_angle * PI / 180)),
                              origin_y - round(hours_hand_length * cos(hours_angle * PI / 180)),
                              0x67E0);

    disp_buffer->fillTriangle(origin_x - round(minutes_hand_width * cos(minutes_angle * PI / 180)),
                              origin_y - round(minutes_hand_width * sin(minutes_angle * PI / 180)),
                              origin_x + round(minutes_hand_width * cos(minutes_angle * PI / 180)),
                              origin_y + round(minutes_hand_width * sin(minutes_angle * PI / 180)),
                              origin_x + round(minutes_hand_length * sin(minutes_angle * PI / 180)),
                              origin_y - round(minutes_hand_length * cos(minutes_angle * PI / 180)),
                              0x031F);

    disp_buffer->fillTriangle(origin_x - round(seconds_hand_width * cos(seconds_angle * PI / 180)),
                              origin_y - round(seconds_hand_width * sin(seconds_angle * PI / 180)),
                              origin_x + round(seconds_hand_width * cos(seconds_angle * PI / 180)),
                              origin_y + round(seconds_hand_width * sin(seconds_angle * PI / 180)),
                              origin_x + round(seconds_hand_length * sin(seconds_angle * PI / 180)),
                              origin_y - round(seconds_hand_length * cos(seconds_angle * PI / 180)),
                              RED);
    disp_buffer->fillCircle(origin_x, origin_y, hours_hand_width + 5, WHITE);
    disp_buffer->fillCircle(origin_x, origin_y, hours_hand_width, BLACK);

    disp_buffer->pushSprite(0, 0);
}

void AnalogClock::render()
{
    StickCP2.Rtc.getTime(&RTC_TimeStruct);
    StickCP2.Rtc.getDate(&RTC_DateStruct);

    // update batt if its changed
    if (get_batt_percentage() != bat_cache)
    {
        render_batt_power();
    }

    // update date if its changed
    if (dt_cache != RTC_DateStruct.date || mn_cache != RTC_DateStruct.month || yr_cache != RTC_DateStruct.year)
    {
        render_date();
    }

    if (s_cache != RTC_TimeStruct.seconds )
    {
        render_clock_face();
    }
}
