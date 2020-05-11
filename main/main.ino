#ifndef UNIT_TEST
  #include <Arduino.h>
#endif

// Infared
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

// Web
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// 433 Mhz
#include <NewRemoteTransmitter.h>

// Sensors
#include <DHT.h>

// Configuration
#include "config.h"

// Templates
#include "homepage_template.h"

// Define Constants
const int pin_ritter = 13;
const int pin_pir  = 14;
const int pin_tmp  = 12;
const uint16_t pin_ir_reciever = 2;
const uint16_t pin_ir_send = 4;
const unsigned long ritter_group_address = 13043702;

// Define variables
decode_results results;

// Setup classes
ESP8266WebServer server ( 80 );
NewRemoteTransmitter transmitter(ritter_group_address, pin_ritter);
DHT dht(pin_tmp, DHT11);
IRrecv irrecv(pin_ir_reciever);
IRsend irsend(pin_ir_send);

/**
 * Associative Arrays aren't possible in C++ because of Memory.
 * For this reason it's necessary to implement the function like this.
 * If you get the return value "UNKNOWN", adapt the return values to the ones in decode_type_t defined in IRremoteESP8266.h
 **/
String getDecodeType(decode_type_t decode_type){
  switch(decode_type){
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
        return String("PANASONIC");
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
  Serial.print(getDecodeType(results->decode_type));
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

void setIrColor() {
  uint32_t code = strtoul(server.arg("code").c_str(), NULL, 10);
  irsend.sendNEC(code, 32);
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

bool isParameterDefined(String parameter_name){
  for (uint8_t parameter_index = 0; parameter_index < server.args(); parameter_index++) {
    if(server.argName(parameter_index)==parameter_name){
      return true;
    }
  }
  return false;
}

void handleRequest(void){
  Serial.println("Website was called.");
  if(isParameterDefined("plug_id") && isParameterDefined("status")){
    if(server.arg("plug_id")=="group"){
      setRitterGroup(server.arg("status").toInt());
    }else{
      setRitterSwitch(server.arg("plug_id").toInt(),server.arg("status").toInt());
    }
  }
  if(server.arg("format")=="json"){
    server.send ( 200, "text/html", getJson());
  }else{
    server.send ( 200, "text/html", homepage_template());
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
