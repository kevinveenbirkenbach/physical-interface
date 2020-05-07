#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NewRemoteTransmitter.h>
#include <DHT.h>
#include <IRremote.h>
#include "config.h"

// Setup pins
const int pin_ritter = 13;
const int pin_pir  = 14;
const int pin_tmp  = 12;
const unsigned long ritter_group_address = 13043702;

// Setup classes
ESP8266WebServer server ( 80 );
NewRemoteTransmitter transmitter(ritter_group_address, pin_ritter);
DHT dht(pin_tmp, DHT11);


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

String getJsonDht(void){
  return "{\"temperature\":\""+String(dht.readTemperature())+"\",\"humidity\":\""+String(dht.readHumidity())+"\"}";
}

String getJsonPir(void){
  return "{\"motion\":\""+String(digitalRead(pin_pir))+"\"";
}

String getJson(void){
  return "{\"DHT\":"+String(getJsonDht())+",\"PIR\":"+String(getJsonPir())+"}";
}

void handleRequest(void){
  if(server.arg("plug_id") && server.arg("status")){
    if(server.arg("plug_id")=="group"){
      setRitterGroup(server.arg("status").toInt());
    }else{
      setRitterSwitch(server.arg("plug_id").toInt(),server.arg("status").toInt());
    }
  }
  if(server.arg("format")=="json"){
    server.send ( 200, "text/html", getJson());
  }else{
    server.send ( 200, "text/html", homepage);
  }
  delay(100);
}

//Arduino-Setup
void setup(void)
{
  pinMode(pin_pir, INPUT);
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
  server.handleClient();
}
