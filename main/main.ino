#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NewRemoteTransmitter.h>
#include <DHT.h>
#include "config.h"

// Define Constants
const int pin_ritter = 13;
const int pin_pir  = 14;
const int pin_tmp  = 12;
const uint16_t pin_ir_reciever = 2;
const unsigned long ritter_group_address = 13043702;

// Define variables
decode_results results;

// Setup classes
ESP8266WebServer server ( 80 );
NewRemoteTransmitter transmitter(ritter_group_address, pin_ritter);
DHT dht(pin_tmp, DHT11);
IRrecv irrecv(pin_ir_reciever);

String getDecodeType(decode_results *results){
  switch(results->decode_type){
      case NEC:
        return String("NEC");
      case SONY:
        return String("SONY");
      case RC5:
        return String("RC5");
      case RC5X:
        return String("RC5X");
      case RC6:
        return String("RC6");
      case RCMM:
        return String("RCMM");
      case PANASONIC:
        return String("PANASONIC" + results->address + HEX);
      case LG:
        return String("LG");
      case JVC:
        return String("JVC");
      case AIWA_RC_T501:
        return String("AIWA_RC_T501");
      case WHYNTER:
        return String("WHYNTER");
  }
  return String("UNKNOWN");
}

void dump(decode_results *results) {
  uint16_t count = results->rawlen;
  Serial.print(getDecodeType(results));
  serialPrintUint64(results->value, 16);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count);
  Serial.print("): ");

  for (uint16_t i = 1; i < count; i++) {
    if (i % 100 == 0)
      yield();  // Preemptive yield every 100th entry to feed the WDT.
    if (i & 1) {
      Serial.print(results->rawbuf[i] * RAWTICK, DEC);
    } else {
      Serial.write('-');
      Serial.print((uint32_t) results->rawbuf[i] * RAWTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}

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
  Serial.begin(9600);
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
  Serial.println("Enable IR-Reciever.");
  irrecv.enableIRIn();
  delay(1000);
}

void loop()
{
  server.handleClient();
  if (irrecv.decode(&results)) {
    dump(&results);
    irrecv.resume();
  }
}
