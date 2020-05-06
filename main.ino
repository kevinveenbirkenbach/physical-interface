#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NewRemoteTransmitter.h>
#include <DHT.h>
#include "config.h"

// Setup ritter actor
const int pin_ritter = 13;
const unsigned long ritter_group_address = 13043702;
NewRemoteTransmitter transmitter(ritter_group_address, pin_ritter);

// Switchs the whole group on
void setRitterGroup(int state)
{
  transmitter.sendGroup(state);
  Serial.print("The state \"");
  Serial.print(state);
  Serial.print("\" was send to the group \"");
  Serial.print(ritter_group_address);
  Serial.println("\".");
}

// Switchs one plug on
void setRitterSwitch(int unit, int state)
{
  transmitter.sendUnit(unit, state);
  Serial.print("The state \"");
  Serial.print(state);
  Serial.print("\" was send to the switch \"");
  Serial.print(unit);
  Serial.println("\".");
}

// Setup DHT sensor
const int pin_tmp  = 12;
DHT dht(pin_tmp, DHT11);

String getJsonDht(void){
  return "{\"temperature\":\""+String(dht.readTemperature())+"\",\"humidity\":\""+String(dht.readHumidity())+"\"}";
}

// Setup pin für PIR
const int pin_pir  = 14;
pinMode(pin_pir, INPUT);

String getJsonPir(void){
  return "{\"motion\":\""+String(digitalRead(pin_pir))+"\"";
}

String getJson(void){
  return "{\"DHT\":"+String(digitalRead(pirPin))+",\"PIR\":"+String(digitalRead(pirPin))+"}";
}

// Setup webserver
ESP8266WebServer server ( 80 );

//Arduino-Setup
void setup(void)
{
  Serial.begin(115200);
  Serial.println("Started program.");
  //WiFi.softAPdisconnect(true);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.print("Connected to :");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.onNotFound(handleRequest);
  server.begin();
  Serial.println("HTTP server started.");
  delay(1000);
}

void loop()
{
  if(server.arg("switch") && server.arg("value")){
    if(server.arg("switch")=="group"){
      setRitterGroup(server.arg("value").toInt());
    }else{
      setRitterSwitch(server.arg("switch").toInt(),server.arg("value").toInt());
    }
  }
  if(server.arg("mode")=="json"){
    server.send ( 200, "text/html", getJson());
  }else{
    server.send ( 200, "text/html", "<html><head><title>Physical Interface</title></head><body>Please check out the <a href=\"https://github.com/kevinveenbirkenbach/physical-interface\">git-repository</a> to get more information about this software.<body><html>");
  }
  delay(100);
}
