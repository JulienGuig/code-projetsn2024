#include <iostream>
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#ifndef tenBotCommandClass
#define tenBotCommandClass

class tenBotCommand {
    private:
        JsonDocument* userSequence = nullptr;
        JsonVariant objectInUse;
        //AsyncWebServer* server = nullptr;
        std::string arg = ""; // argument of the command

    public:
        tenBotCommand(JsonDocument& userSequenceJSON); //, AsyncWebServer& ESP_server

        void initCommand(std::string cmd);
        void initCommand(String& cmd);
        void initCommand(const char* cmd);

};

#endif