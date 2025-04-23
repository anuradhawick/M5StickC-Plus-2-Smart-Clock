#pragma once

#include <M5StickCPlus2.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "View.h"
#include "animations/loader.h"
#include "util.h"
#include "data.h"

class APView : public View
{
public:
    APView();
    ~APView();

    void render();
    bool receive_event(EVENTS::event event) { return false; };

private:
    void render_loader();
    void render_text();

    unsigned long loader_last_rendered;
    unsigned long text_last_rendered;
    unsigned long inited_time;

    uint16_t loader_frame;
    M5Canvas *disp_buffer;
    String ip_address;
    WebServer *server;
};
