#include <M5StickCPlus2.h>
#include "views/View.h"
#include "views/TimeView.h"

static const char *TAG = "TIME_VIEW";
extern String days[7];

TimeView::TimeView()
{
    M5.begin();
    M5.Display.setRotation(3);
    M5.Display.setSwapBytes(false);

    disp_buffer = new M5Canvas(&StickCP2.Display);

    disp_buffer->setSwapBytes(false);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);
    disp_buffer->pushSprite(0, 0);

    disp_buffer->setTextSize(1);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);
}

TimeView::~TimeView()
{
    ESP_LOGD(TAG, "Destructor called");
    disp_buffer->deleteSprite();
    delete disp_buffer;
    ESP_LOGD(TAG, "Destructor finished");
}

void TimeView::render()
{
    m5::rtc_time_t timeStruct;
    m5::rtc_date_t dateStruct;
    StickCP2.Rtc.getTime(&timeStruct);
    StickCP2.Rtc.getDate(&dateStruct);
    disp_buffer->setCursor(0, 15);
    disp_buffer->setTextFont(0);
    disp_buffer->setFont(&DSEG7_Classic_Regular_64);

    // if time has changed
    if (h_cache != timeStruct.hours || m_cache != timeStruct.minutes)
    {
        h_cache = timeStruct.hours;
        m_cache = timeStruct.minutes;
        sprintf(timeHHMM, "%02d:%02d", h_cache, m_cache);
        ESP_LOGD(TAG, "Updating time to - %s", timeHHMM);
        disp_buffer->drawString(timeHHMM, 5, 60);
    }

    // if seconds has changed
    if (s_cache != timeStruct.seconds)
    {
        s_cache = timeStruct.seconds;
        sprintf(seconds, "%02d", s_cache);
        ESP_LOGD(TAG, "Updating seconds to - %s", seconds);
        disp_buffer->setFont(&DSEG7_Classic_Bold_30);
        disp_buffer->drawString(seconds, 175, 5);
    }

    // if day has changed
    if (d_cache != dateStruct.weekDay)
    {
        d_cache = dateStruct.weekDay;
        sprintf(day, "%-10s", days[d_cache].c_str());
        disp_buffer->drawString(day, 10, 5, &fonts::Font4);
    }

    // update date if its changed
    if (dt_cache != dateStruct.date || mn_cache != dateStruct.month || yr_cache != dateStruct.year)
    {
        dt_cache = dateStruct.date;
        mn_cache = dateStruct.month;
        yr_cache = dateStruct.year;
        disp_buffer->setTextColor(grey, TFT_BLACK);
        disp_buffer->drawString(String(dt_cache) + "/" + String(mn_cache), 10, 30, &fonts::Font4);
        disp_buffer->drawString(String(yr_cache), 70, 35, &fonts::Font2);
        disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);
    }

    disp_buffer->pushSprite(0, 0);
}
