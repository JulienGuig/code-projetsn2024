#include <iostream>
#include <string>

#include "ballClass.h"
#include "sequenceClass.h"
#include "tenBotCommandClass.h"

#include <WiFi.h>
#include <WiFiAP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

using namespace std;

// network credidentals
const char* ssid = "tenBot ssid";
const char* password = "lePassword";

AsyncWebServer server(80);
JsonDocument jsonDoc;

String userInUse = "";
String sequenceInUse = "";

// used to communicate the number of ball and the sequence to send in the void loop() and know when to send
uint8_t currentBall = 64; // initialazed to a high value to not provoque a sequence reset in the void loop()
sequence comSq;

////////////////////////////////////////////////////////////////////////////////

// ball value viewer
void testBallValue(ball& Ball)
{
  cout << "type: " << Ball.getBallType() << " || ";
  cout << "velocity: " << Ball.getBallVelocity() <<  " || ";
  cout << "effect: " << Ball.getBallEffect() <<  " || ";
  cout << "pan: " << Ball.getShootPan() <<  " || ";
  cout << "tilt: " << Ball.getShootTilt() <<  " || ";
  cout << "wait: " << Ball.getWait() << endl;
}

// replace all wanted characters to another in a string
void replaceAll(string& std, const string target, const string replacement)
{
  unsigned short pos = 0;
  while(std.find(target,pos) != string::npos){
    std.replace(std.find(target,pos), target.length(), replacement);
    pos++;
  }
}

//////////////////////////////////////////////////////////////////////
void setup() {
  /* -------- SETUP initialization -------- */
  Serial1.begin(9600, SERIAL_8N1, 16, 17);  // send value to the atmega328p
  Serial.begin(115200); // debug on the serial monitor
  
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // !! a valid password must have more than 7 characters !!
  if (!WiFi.softAP(ssid, password, 1, 0, 1)) {
    Serial.println("");
    Serial.println("Soft AP creation failed");
    log_e("Soft AP creation failed.");
    while(true);
  }

  if (!SPIFFS.begin(true)) 
  { 
    Serial.println("An Error has occurred while mounting SPIFFS");
    while(true); 
  }


        /* --- Initialized (send) web pages -- */
//
  // send the page notFound to the user when he is on an unknow page
  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.println("tenBot-ESP32: entering not found");
    request->send(SPIFFS, "/notFound.html");
  });


  // send the home page to the client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("tenBot-ESP32: sending index.html page to the client");
    request->send(SPIFFS, "/index.html");
  });


  // send changeSequence file sequence on the client
  server.on("/modif", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("tenBot-ESP32: client enter /modif");

    if(request->hasParam("HTML_userName") && request->hasParam("HTML_userSqName")) {
      userInUse = String(request->getParam("HTML_userName")->value());
      sequenceInUse = String(request->getParam("HTML_userSqName")->value());

      request->send(SPIFFS, "/changeSequence.html");
    }
    else {
      request->send(400, "text/html", "<h1>error: the form don't have the correct parameter</h1>");
      Serial.println("Err in /modif no request");
    }
  });


  // send the sequence creator html file on the client
  server.on("/create", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("tenBot-ESP32: sending createSequence.html page to the client"); 
    request->send(SPIFFS, "/createSequence.html");
  });
//

        /* --- do stuff in the pages -- */
//
  // send the information about the sequence
  server.on("/change", HTTP_GET, [](AsyncWebServerRequest* request) {
      File sqFileRead = SPIFFS.open("/userSequence.json", FILE_READ);
      String sqFileContent = sqFileRead.readString();
      deserializeJson(jsonDoc, sqFileContent);
      sqFileRead.close();

      if(jsonDoc.containsKey(userInUse)) {
        if(jsonDoc[userInUse].containsKey(sequenceInUse)) {
          Serial.println("Sending dataFrame: ");
          String dataframe =  userInUse + ',' + sequenceInUse + '/' + jsonDoc[userInUse][sequenceInUse].as<String>();
          Serial.println(dataframe);

          request->send(200, "text/plain", dataframe);
        }
        else {
          request->send(400, "text/html", "<h1>error: this sequence does not exist</h1>");
          Serial.println("Err unknow sequence");
        }
      }
      else {
        request->send(400, "text/html", "<h1>error: this user does not exist</h1>");
        Serial.println("Err unknow user");
      }
  });


  // html get request https://github.com/me-no-dev/ESPAsyncWebServer/blob/master/examples/simple_server/simple_server.ino
  // get all information on the form and create/modify the user and/or the sequence
  server.on("/new", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("tenBot-ESP32: client enter /new");
    userInUse = "";
    sequenceInUse = "";

    if(request->hasParam("HTML_btnNum") && request->hasParam("HTML_userName") && request->hasParam("HTML_userSqName")) {
      const uint8_t nbBall = (short)(request->getParam("HTML_btnNum")->value().toInt());
      userInUse = String(request->getParam("HTML_userName")->value());
      sequenceInUse = String(request->getParam("HTML_userSqName")->value());
      //Serial.print("btnNum: "); Serial.println(nbBall);
      
      // creating a sequence with no already initialized balls
      sequence ballSq;
      
      // repeat for the number that was in "HTML_btnNum" form parameter value
      for(unsigned short nDiv=0; nDiv<=nbBall; nDiv++) {
        // create the getParam request
        String htmlParam("HTML_BN");
        htmlParam += nDiv;

        if(request->hasParam(htmlParam)){
          const String paramRequest(request->getParam(htmlParam)->value());
          string inputMessage(paramRequest.c_str());

          // replace some characters in the get request with the corresponding character in the dataframe
          replaceAll(inputMessage, "%21", "!");
          replaceAll(inputMessage, "%2C", ",");
          replaceAll(inputMessage, "%3A", ":");
          
          ball tempBall(inputMessage);
          ballSq.insertBall(tempBall);

          Serial.print("ball inserted, here is value: ");
          testBallValue(ballSq.getBallAt(nDiv));
        }
        else {
          request->send(400, "text/html", (String)"<h1>error: the form don't have the correct parameter: no " + htmlParam + (String)" param</h1>");
          Serial.println((String)"Err: the form don't have the correct parameter: no " + htmlParam + (String)" param");
        }
      } //end for loop

      // saving value to Json in a spiffs file
      // deserialize
      File sqFileRead = SPIFFS.open("/userSequence.json", FILE_READ);
      String sqFileContent = sqFileRead.readString();
      deserializeJson(jsonDoc, sqFileContent);
      sqFileRead.close();

      // remove temporarely user sequence data to proporly change it (solve bug where the number of ball in the sequence can only go up)
      JsonObject userSq = jsonDoc[userInUse];
      if(userSq.containsKey(sequenceInUse)) {
        userSq.remove(sequenceInUse);
      }

      // changing value
      for(unsigned short nDiv=0; nDiv<=nbBall; nDiv++) {
        jsonDoc[userInUse][sequenceInUse][nDiv]["type"] = ballSq.getBallAt(nDiv).getBallType();
        jsonDoc[userInUse][sequenceInUse][nDiv]["velocity"] = ballSq.getBallAt(nDiv).getBallVelocity();
        jsonDoc[userInUse][sequenceInUse][nDiv]["effect"] = ballSq.getBallAt(nDiv).getBallEffect();
        jsonDoc[userInUse][sequenceInUse][nDiv]["pan"] = ballSq.getBallAt(nDiv).getShootPan();
        jsonDoc[userInUse][sequenceInUse][nDiv]["tilt"] = ballSq.getBallAt(nDiv).getShootTilt();
        jsonDoc[userInUse][sequenceInUse][nDiv]["wait"] = ballSq.getBallAt(nDiv).getWait();
      }    

      Serial.print('"');
      Serial.print(userInUse);
      Serial.print('"');
      Serial.println(" user successfully added/changed: ");  
      serializeJsonPretty(jsonDoc[userInUse][sequenceInUse], Serial);
      Serial.println("");
      
      // serialize 
      File sqFile = SPIFFS.open("/userSequence.json", FILE_WRITE);
      serializeJson(jsonDoc, sqFile);
      
      sqFile.close();

      Serial.print("userSequence size change to: ");
      Serial.print(measureJson(jsonDoc));
      Serial.println(" bytes");

      // send an html response so the browser doesn't reload the get request and therefore re-create the sequence
      request->send(200, "text/html", "<h1>succesfully sended</h1>");
    }
    else {
      String errStd="";
      if(!request->hasParam("HTML_btnNum")) {
        errStd+="no HTML_btnNum ";
      }
      if(!request->hasParam("HTML_userName")) {
        errStd+="no HTML_userName ";
      }
      if(!request->hasParam("HTML_userSqName")) {
        errStd+="no HTML_userSqName ";
      }
      request->send(400, "text/html", (String)"<h1>error: the form don't have the correct parameter: " + errStd + (String)"</h1>");
      Serial.println("Err incorect request");
      
    }
  }); //end server.on(/new)


  server.on("/play", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("tenBot-ESP32: client enter /play");
    request->send(200, "text/html", (String)"<h1>playing</h1>");

    if(request->hasParam("HTML_btnNum")) {
      const uint8_t nbBall = (short)(request->getParam("HTML_btnNum")->value().toInt());
      //Serial.print("btnNum: "); Serial.println(nbBall);
      
      // repeat for the number that was in "HTML_btnNum" form parameter value
      for(unsigned short nDiv=0; nDiv<=nbBall; nDiv++) {
        // create the getParam request
        String htmlParam("HTML_BN");
        htmlParam += nDiv;

        if(request->hasParam(htmlParam)){
          const String paramRequest(request->getParam(htmlParam)->value());
          string inputMessage(paramRequest.c_str());

          // replace some characters in the get request with the corresponding character in the dataframe
          replaceAll(inputMessage, "%21", "!");
          replaceAll(inputMessage, "%2C", ",");
          replaceAll(inputMessage, "%3A", ":");
          
          ball tempBall(inputMessage);
          comSq.insertBall(tempBall);

          Serial.print("ball inserted, here is value: ");
          testBallValue(comSq.getBallAt(nDiv));
        }
        else {
          request->send(400, "text/html", (String)"<h1>error: the form don't have the correct parameter: no " + htmlParam + (String)" param</h1>");
          Serial.println((String)"Err: the form don't have the correct parameter: no " + htmlParam + (String)" param");
        }
      } //end for loop

      ball endBall(BallType::jaune, 0, 0, 0, 0, 2000);
      comSq.insertBall(endBall);

      // send the data to the atmega328p
      Serial.println("begin to send value");
      currentBall=0; // put to 0 to say that we can begin sending the sequence in the void loop()
    }
    else {
      request->send(400, "text/html", (String)"<h1>error: the form don't have the correct parameter: no HTML_btnNum</h1>");
      Serial.println("Err incorect request");
    }
  }); //end server.on(/play)


  /* -------- SETUP actual setup -------- */

  // load jsonDoc after esp reboot
  File sqFileRead = SPIFFS.open("/userSequence.json", FILE_READ);
  String sqFileContent = sqFileRead.readString();
  deserializeJson(jsonDoc, sqFileContent);
  sqFileRead.close();

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
  
  Serial.println("Server started");
  Serial.println("end setup");

}

// initialize tenBotCommand
tenBotCommand tenBotCmd(jsonDoc);
unsigned long int beginSendTime = millis();

////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  if (Serial.available() > 0) {
    string input((Serial.readString()).c_str());
    if(input.length() > 1 && input.substr(0,1) == "/") {
      input = input.substr(1, input.length()-1);
      if(input.substr(0,1) == "$") {
        Serial1.println(input.c_str());
        Serial.print("sended:");Serial.println(input.c_str());
      }
      else  {
        tenBotCmd.initCommand('/'+input);
      }
    }
  }


  // prepare and send data to the main serial communication
  if(comSq.getSize() >= 1 && (currentBall<comSq.getSize())) {
    unsigned long int sendDelay = millis();
    if(sendDelay-beginSendTime > comSq.getBallAt((currentBall > 0) ? currentBall-1 : 0).getWait()) {
      Serial.println("-----------");
      // high motor speed
      String df = "$VH(";
      short val = comSq.getBallAt(currentBall).getBallVelocity() + (comSq.getBallAt(currentBall).getBallEffect()*comSq.getBallAt(currentBall).getBallEffect());
      if(val > BALL_VELOCITY_MAX) {
        val = BALL_VELOCITY_MAX;
      }
      else if(val < 20 && currentBall != comSq.getSize()-1)  {
        val = 20;
      }
      df+=String(val);
      df+=(String)')';
      Serial1.print(df);
      Serial.print("sended:"); Serial.println(df);

      // low motor speed
      df="$VL(";
      val = comSq.getBallAt(currentBall).getBallVelocity() - (comSq.getBallAt(currentBall).getBallEffect()*comSq.getBallAt(currentBall).getBallEffect());
      if(val > BALL_VELOCITY_MAX) {
        val = BALL_VELOCITY_MAX;
      }
      else if(val < 20 && currentBall != comSq.getSize()-1) {
        val = 20;
      }
      df+=String(val);
      df+=(String)')';
      Serial1.print(df);
      Serial.print("sended:"); Serial.println(df);

      // pan angle
      df="$X";
      val = (comSq.getBallAt(currentBall).getShootPan() - ((currentBall > 0) ? comSq.getBallAt(currentBall-1).getShootPan() : 0)) * 200;
      (val < 0) ? df+=(String)"g(" + (String)(abs(val)) : df+=(String)"d(" + (String)(abs(val));
      df+=(String)')';
      Serial1.print(df);
      Serial.print("sended:"); Serial.println(df);

      // tilt angle
      df="$Y";
      val = (comSq.getBallAt(currentBall).getShootTilt() - ((currentBall > 0) ? comSq.getBallAt(currentBall-1).getShootTilt() : 0)) * 200;
      (val < 0) ? df+=(String)"g(" + (String)(abs(val)) : df+=(String)"d(" + (String)(abs(val));
      df+=(String)')';
      Serial1.print(df);
      Serial.print("sended:"); Serial.println(df);

      // reset
      beginSendTime = sendDelay;
      currentBall++;
    }
      
  }
  else if(currentBall == comSq.getSize() && comSq.getSize() >= 1) {
    comSq.resetSequence();
    currentBall = 64;
    Serial.println("reseting sequence, ESP32 is ready to send another sequence");
  }

}
