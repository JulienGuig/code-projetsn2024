#include <iostream>
#include <string>

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#include "ballClass.h"
#include "sequenceClass.h"
#include "tenBotCommandClass.h"

using namespace std;

tenBotCommand::tenBotCommand(JsonDocument& userSequenceJSON) //, AsyncWebServer& ESP_server
{
    userSequence = &userSequenceJSON;
    //server = &ESP_server;
}

// main method
void tenBotCommand::initCommand(string cmd)
{
    string result = "is either unknow or not properly written"; // result of the command (eg. error)
    size_t search; // used to find command that require argument

    Serial.println();

    if(cmd == "/showAllSequence") {
        serializeJsonPretty(*userSequence, Serial);
        Serial.println();
        result = "done";
    }

    if(cmd == "/resetUserSequence") {
        userSequence->clear();
        File sqFile = SPIFFS.open("/userSequence.json", FILE_WRITE); //save in the file
        serializeJson(*userSequence, sqFile);
        sqFile.close();
        result = "done";
    }

    search = cmd.find("/select ");
    if(search != string::npos) { 
        if(search == 0 && cmd.find_last_of("/") == 0) { //look if the command is valid
            arg = cmd.substr(search+8);
            if(!objectInUse.isNull()) {
                if(objectInUse.containsKey(arg)) { // uses of older objectInUse if command repeat
                    objectInUse = objectInUse.operator[](arg);
                    result = "selected:" + arg + "\ndone";
                }
            }
            else if(userSequence->containsKey(arg)) {
                objectInUse = userSequence->operator[](arg);
                result = "selected:" + arg + "\ndone";
            }
            else {
                result = "Command error: selection doesn't exit";
            }
        }
        else {
            result = "missing argument or mutiple commands detected";
        }
    }

    if(cmd == "/showSelect") {
        if(arg == "") {
            result = "Command error: you must select something first";
        }
        else if(!objectInUse.isNull()) {
            serializeJsonPretty(objectInUse, Serial);
            Serial.println();
            Serial.print("size: ");
            Serial.println(objectInUse.size());
            result = "done";
        }
        else {
            result = "error selection might be wrong";
        }
    }

    search = cmd.find("/deleteUser ");
    if(search != string::npos) { 
        if(search == 0 && cmd.find_last_of("/") == 0) { //look if the command is valid
            arg = cmd.substr(search+12);
            if(userSequence->containsKey(arg)) {
                userSequence->remove(arg);
                File sqFile = SPIFFS.open("/userSequence.json", FILE_WRITE);
                serializeJson(*userSequence, sqFile);
                sqFile.close();  
                arg = "";
                result = "done";
            }
            else {
                result = "Command error: user doesn't exist";
            }
        }
        else {
            result = "missing argument or mutiple commands detected";
        }
    }
    
    search = cmd.find("/deleteInSelect ");
    if(search != string::npos) {
        if(search == 0 && cmd.find_last_of("/") == 0) { //look if the command is valid
            arg = cmd.substr(search+16);
            if(!objectInUse.isNull()) {
                objectInUse.remove(arg);
                File sqFile = SPIFFS.open("/userSequence.json", FILE_WRITE);
                serializeJson(*userSequence, sqFile);
                sqFile.close();
                arg = "";
                result = "done";
            }
            else {
                result = "Command error: you must select something first";
            }
        }
        else {
            result = "missing argument or mutiple commands detected";
        }
    }

    if(cmd == "/unselect") {
        arg = "";
        JsonVariant newObject;
        objectInUse = newObject; // reset select by reseting objectInUse
        result = "done";
    }
    
    search = cmd.find("/listDir ");
    if(search != string::npos) {
        if(search == 0) { //look if the command is valid
            arg = cmd.substr(search+9);
            const char* argc = arg.c_str();

            result = "error";
            // file explorer (original source asn't been found and was changed a bit)
            File root = SPIFFS.open(argc);
            if(!root){
                result = "error: failed to open directory";
            }
            if(!root.isDirectory()){
                result = "Command error: " + arg + " is not a directory";
            }

            if(root && root.isDirectory()) {
                Serial.print("Listing directory: "); Serial.println(argc);
                File file = root.openNextFile();
                result = (file) ? "done" : "doesn't exist";
                while(file){
                    if(file.isDirectory()){
                        Serial.print("  DIR : ");
                        Serial.println(file.name());
                    } else {
                        Serial.print("  FILE: ");
                        Serial.print(file.name());
                        Serial.print("\tSIZE: ");
                        Serial.print(file.size());
                        Serial.println((String)" byte" + (String)((file.size() > 1) ? "s" : ""));
                    }
                    file = root.openNextFile();
                }
                file.close();
            }
            root.close();

            arg = "";
            Serial.println();
        }
        else {
            result = "missing argument or mutiple commands detected";
        }
    }

    search = cmd.find("/readFile ");
    if(search != string::npos) {
        if(search == 0) { //look if the command is valid
            arg = cmd.substr(search+10);
            File readFile = SPIFFS.open(arg.c_str(), FILE_READ);
            if(SPIFFS.exists(arg.c_str())) {
                while(readFile.available()){
                    Serial.write(readFile.read());
                }
                result = "done";
            }
            else {
                result = "Command error: " + arg + " doesn't exist";
            }

            readFile.close();
            arg = "";
            Serial.println();
        }
        else {
            result = "missing argument or mutiple commands detected";
        }
    }

    if(cmd == "/IP") {
        Serial.print("AP IP address: "); Serial.println(WiFi.softAPIP());
        Serial.print("AP MAC address: "); Serial.println(WiFi.softAPmacAddress());
        result = "done";
    }

    if(cmd == "/spiffsMemory") {
        Serial.println("SPIFFS memory:");
        Serial.print(SPIFFS.usedBytes());
        Serial.print(" bytes / ");
        Serial.print(SPIFFS.totalBytes());
        Serial.print(" bytes      (");
        Serial.print((float)SPIFFS.usedBytes()/(float)SPIFFS.totalBytes());
        Serial.println("%)");
        result = "done";
    }

    if(cmd == "/espInfo") {
        Serial.print("total heap size: "); Serial.println(ESP.getHeapSize());
        Serial.print("available heap: "); Serial.println(ESP.getFreeHeap());
        Serial.print("lowest level of free heap since boot: "); Serial.println(ESP.getMinFreeHeap());
        Serial.print("largest block of heap that can be allocated at once: "); Serial.println(ESP.getMaxAllocHeap());

        Serial.print("psram size: "); Serial.println(ESP.getPsramSize());
        Serial.print("free psram: "); Serial.println(ESP.getFreePsram());
        Serial.print("min free psram: "); Serial.println(ESP.getMinFreePsram());
        Serial.print("max alloc psram: "); Serial.println(ESP.getMaxAllocPsram());

        Serial.print("chip revision: "); Serial.println(ESP.getChipRevision());
        Serial.print("chip model: "); Serial.println(ESP.getChipModel());
        Serial.print("chip cores: "); Serial.println(ESP.getChipCores());
        Serial.print("cpu frequency: "); Serial.print(ESP.getCpuFreqMHz()); Serial.println(" MHz");
        Serial.print("cycle count: "); Serial.println(ESP.getCycleCount());

        Serial.print("sdk version: "); Serial.println(ESP.getSdkVersion());
        Serial.print("core version: "); Serial.println(ESP.getCoreVersion());

        Serial.print("flash chip size: "); Serial.println(ESP.getFlashChipSize());
        Serial.print("flash chip speed: "); Serial.println(ESP.getFlashChipSpeed());
        Serial.print("flash chip mode: "); Serial.println(ESP.getFlashChipMode());

        Serial.print("sketch size: "); Serial.println(ESP.getSketchSize());
        Serial.print("sketch MD5: "); Serial.println(ESP.getSketchMD5());
        Serial.print("free sketch space: "); Serial.println(ESP.getFreeSketchSpace());

        Serial.print("efuse mac: "); Serial.println(ESP.getEfuseMac());
        result = "done";
    }

    if(cmd == "/test") {
        File testFile = SPIFFS.open("/userSequence.json", FILE_READ);
        Serial.print("/userSequence.json size:"); Serial.println(testFile.size());
        testFile.close();
        Serial.print("measurejson size:"); Serial.println(measureJson(*userSequence));
        Serial.print("sizeof usersequence:"); Serial.println(sizeof(*userSequence));

        result = "done";
    }

    //keep this at the end of the method
    Serial.print(cmd.c_str());
    Serial.print(" : ");
    Serial.println(result.c_str());
}


void tenBotCommand::initCommand(String& cmd)
{
    initCommand(string(cmd.c_str()));
}

void tenBotCommand::initCommand(const char* cmd)
{
    initCommand(string(cmd));
}