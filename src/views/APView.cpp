#include "views/APView.h"

static const char *TAG = "AP_VIEW";

APView::APView()
{
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(true);

    disp_buffer = new M5Canvas(&M5.Lcd);
    server = new WebServer(80);
    WiFi.enableAP(true);
    WiFi.softAP("M5StickC-AP", "12345678");

    disp_buffer->setSwapBytes(true);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);
    disp_buffer->pushSprite(0, 0);

    disp_buffer->setTextSize(1);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);

    loader_last_rendered = 0;
    text_last_rendered = 0;
    loader_frame = 0;

    if (!LittleFS.begin())
    {
        ESP_LOGE(TAG, "LittleFS Mount Failed");
        return;
    }

    server->on("/", HTTP_GET, [this]()
               {
        File index_html = LittleFS.open("/index.html", "r");
        server->streamFile(index_html, "text/html");
        index_html.close(); });

    server->on("/settings", HTTP_GET, [this]()
               {
        JsonDocument doc = storage.get_json();
        String json;
        serializeJson(doc, json);
        server->send(200, "application/json", json.c_str()); });

    server->on("/settings", HTTP_POST, [this]()
               {
        if (server->hasArg("plain"))
        {
            String json = server->arg("plain");
            ESP_LOGD(TAG, "Settings: %s", json.c_str());
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, json);
            if (error)
            {
                ESP_LOGE(TAG, "Failed to parse JSON: %s", error.c_str());
                server->send(400, "application/json", "{\"success\":false}");
                return;
            }
            storage.set_json(doc);
        }
        server->send(200, "application/json", "{\"success\":true}"); });

    server->on("/networks", HTTP_GET, [this]()
               {
        String json;
        int n = WiFi.scanNetworks();
        if (n == 0)
        {
            json = "[]";
        }
        else
        {
            JsonDocument doc;
            JsonArray array = doc.to<JsonArray>();
            for (int i = 0; i < n; i++)
            {
                JsonObject obj = array.add<JsonObject>();
                obj["ssid"] = WiFi.SSID(i);
                obj["rssi"] = WiFi.RSSI(i);
                obj["encryption"] = WiFi.encryptionType(i);
            }
            serializeJson(doc, json);
        }
        server->send(200, "application/json", json.c_str()); });

    inited_time = millis();
    server->begin();
    render_text();
    render_loader();
}

APView::~APView()
{
    ESP_LOGD(TAG, "Destructor called");
    disconnect_wifi();

    disp_buffer->deleteSprite();
    delete disp_buffer;

    server->stop();
    delete server;

    WiFi.softAPdisconnect(true);

    ESP_LOGD(TAG, "Destructor finished");
}

void APView::render_loader()
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

void APView::render_text()
{
    disp_buffer->fillRect(0, 0, 140, 135, BLACK);
    disp_buffer->setCursor(0, 15);
    disp_buffer->setTextFont(0);
    disp_buffer->drawString("AP", 10, 10, &fonts::Font4);
    disp_buffer->drawString("Started", 10, 30, &fonts::Font4);
    disp_buffer->drawString("IP " + WiFi.softAPIP().toString(), 10, 70, &fonts::Font2);
    disp_buffer->drawString("SSID: M5StickC-AP", 10, 90, &fonts::Font2);
    disp_buffer->drawString("PASS: 12345678", 10, 110, &fonts::Font2);
    disp_buffer->pushSprite(0, 0);
}

void APView::render()
{
    server->handleClient();
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
}
