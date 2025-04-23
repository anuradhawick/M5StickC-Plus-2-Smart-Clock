#pragma once

#include <M5StickCPlus2.h>
#include "View.h"

#define grey 0x65DB

class BrightnessView : public View
{
public:
    BrightnessView();
    ~BrightnessView();

    void render();
    bool receive_event(EVENTS::event event);

private:
    M5Canvas *disp_buffer;
    uint8_t brightness;
    uint8_t rendering_brightness_value;
    String header_message;
    unsigned long last_rendered;
};
