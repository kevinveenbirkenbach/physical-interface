/**
 * Loading classes
 */
#ifndef UNIT_TEST
  #include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NewRemoteTransmitter.h>
#include <DHT.h>
#include "config.h"

/**
 * Define constants
 */
const uint16_t PIN_RITTER = 13;
const uint16_t PIN_PIR  = 14;
const uint16_t PIN_DHT  = 12;
const uint16_t PIN_IR_RECIEVER = 2;
const uint16_t PIN_IR_SEND = 4;
const uint16_t PIN_LDR  = A0;
const uint16_t PIN_ACTIVE_BUZZER  = D5;
const unsigned long RITTER_STANDART_GROUP_ADDRESS = 13043702;
const char* PARAMETER_PLUG_ID="plug_id";
const char* PARAMETER_PLUG_STATUS="plug_enabled";
const char* PARAMETER_IR_TYPE="ir_type";
const char* PARAMETER_IR_CODE="ir_code";
const char* PARAMETER_IR_BITS="ir_bits";
const char* PARAMETER_PRE_DELAY_TIME_IN_MS="pre_delay_time_in_ms";
const char* PARAMETER_SOUND="sound_enabled";
const char* PARAMETER_LIST[]={PARAMETER_PLUG_ID,PARAMETER_PLUG_STATUS,PARAMETER_IR_TYPE,PARAMETER_IR_CODE,PARAMETER_IR_BITS,PARAMETER_PRE_DELAY_TIME_IN_MS,PARAMETER_SOUND};

/**
 * Define variables
 */
decode_results results;
decode_type_t last_recieved_ir_type;
int last_recieved_ir_code;
int last_recieved_ir_bits;

/**
 * Setup classes
 */
MDNSResponder mdns;
ESP8266WebServer server ( 80 );
NewRemoteTransmitter transmitter(RITTER_STANDART_GROUP_ADDRESS, PIN_RITTER);
DHT dht(PIN_DHT, DHT11);
IRrecv irrecv(PIN_IR_RECIEVER);
IRsend irsend(PIN_IR_SEND);

/**
 * Setter functions
 */
void setRecievedIr(decode_results *results) {
  last_recieved_ir_type = results->decode_type;
  last_recieved_ir_code = results->value;
  last_recieved_ir_bits = results->bits;
  Serial.println("The code \"" + String(last_recieved_ir_code) + "\" was send for type \"" + String(results->decode_type,DEC) + "\" with  \"" + String(results->bits,DEC) + "\" bits.");
}

/**
 * Transmitter functions
 */
void sendRemoteGroupSignal(boolean state)
{
  transmitter.sendGroup(state);
  Serial.println("The state \"" + String(state,BIN) + "\" was send to the group \"" + String(RITTER_STANDART_GROUP_ADDRESS,DEC) + "\".");
}

void sendRemoteUnitSignal(int unit, boolean state)
{
  transmitter.sendUnit(unit, state);
  Serial.println("The state \"" + String(state,BIN) + "\" was send to the switch \"" + String(unit,DEC) + "\".");
}

void sendIrCode(decode_type_t type,uint32_t code, uint16_t bits) {
  irsend.send(type, code, bits);
  Serial.println("The code \"" + String(code) + "\" with \"" + String(bits) + "\" bits was send in format \"" + String(type) + "\".");
}

/**
 * Actors
 */
void switchSound(boolean status){
  Serial.println("Switching sound \"" + String((status)?("on"):("off")) + ".");
  if(status){
    pinMode(PIN_ACTIVE_BUZZER,OUTPUT);
    digitalWrite(PIN_ACTIVE_BUZZER,LOW);
  }else{
    digitalWrite(PIN_ACTIVE_BUZZER,HIGH);
    pinMode(PIN_ACTIVE_BUZZER,INPUT);
  }
}

/**
 * Helper functions
 */
bool isParameterDefined(String parameter_name){
  for (uint8_t parameter_index = 0; parameter_index < server.args(); parameter_index++) {
    if(server.argName(parameter_index)==parameter_name){
      return true;
    }
  }
  return false;
}

String getParameterType(const char* parameter){
  if(
    parameter==PARAMETER_PLUG_ID ||
    parameter==PARAMETER_IR_TYPE ||
    parameter==PARAMETER_IR_CODE ||
    parameter==PARAMETER_PRE_DELAY_TIME_IN_MS ||
    parameter==PARAMETER_IR_BITS ||
    parameter==PARAMETER_PLUG_ID
  ){
    return "integer";
  }

  if(
    parameter==PARAMETER_SOUND ||
    parameter==PARAMETER_PLUG_STATUS
  ){
    return "boolean";
  }
  return "text";
}

void controller(void){
  switchSound(server.arg(PARAMETER_SOUND).equals("on"));
  if(isParameterDefined(PARAMETER_IR_TYPE) && isParameterDefined(PARAMETER_IR_CODE) && isParameterDefined(PARAMETER_IR_BITS)){
    sendIrCode(static_cast<decode_type_t>(server.arg(PARAMETER_IR_TYPE).toInt()),server.arg(PARAMETER_IR_CODE).toInt(),server.arg(PARAMETER_IR_BITS).toInt());
  }
  if(isParameterDefined(PARAMETER_PLUG_ID)){
      if(server.arg(PARAMETER_PLUG_ID).equals("0")){
        sendRemoteGroupSignal(server.arg(PARAMETER_PLUG_STATUS).equals("on"));
      }else if(server.arg(PARAMETER_PLUG_ID).toInt()>0){
        sendRemoteUnitSignal(server.arg(PARAMETER_PLUG_ID).toInt(),server.arg(PARAMETER_PLUG_STATUS).equals("on"));
      }
  }
}

/**
 * Getter functions
 */
String getJsonDht(void){
  return "{\"temperature_celcius\":\""+String(dht.readTemperature())+"\",\"humidity\":\""+String(dht.readHumidity())+"\"}";
}

String getJsonPir(void){
  return "{\"motion\":\""+String(digitalRead(PIN_PIR))+"\"}";
}

String getJsonLdr(void){
  float volt = 5.0 /1024.0 * analogRead (PIN_LDR);
  return "{\"input_volt\":\""+String(volt)+"\"}";
}

String getJsonIrLastRecieved(void){
  return "{\"last_recieved\":{\"bits\":\""+String(last_recieved_ir_bits)+"\",\"type\":\""+String(last_recieved_ir_type)+"\",\"data\":\""+String(last_recieved_ir_code)+"\"}}";
}

String getJson(void){
  return "{\"LDR\":"+String(getJsonLdr())+",\"DHT\":"+String(getJsonDht())+",\"PIR\":"+String(getJsonPir())+",\"IR\":"+String(getJsonIrLastRecieved())+"}";
}

#include "homepage_template.h"

void view(void){
  if(server.arg("format")=="json"){
    server.send ( 200, "text/html", getJson());
  }else{
    server.send ( 200, "text/html", homepage_template());
  }
}

int getDelayTime(void){
  if(isParameterDefined(PARAMETER_PRE_DELAY_TIME_IN_MS)){
      int delay_time_in_ms = server.arg(PARAMETER_PRE_DELAY_TIME_IN_MS).toInt();
      if(delay_time_in_ms>0){
        Serial.println("Applying delay time: " + server.arg(PARAMETER_PRE_DELAY_TIME_IN_MS) + "ms");
        return delay_time_in_ms;
      }
  }
  return 0;
}

void handleRequest(void){
  Serial.println("Website was called.");
  delay(getDelayTime());
  controller();
  view();
}

//Arduino-Setup
void setup(void)
{
  pinMode(PIN_PIR, INPUT);
  Serial.println("Enable IR-reciever.");
  irrecv.enableIRIn();
  Serial.println("Enable IR-sender.");
  irsend.begin();
  Serial.println("Activate active buzzer.");
  switchSound(true);
  Serial.begin(9600);
  Serial.println("Started program.");
  //WiFi.softAPdisconnect(true);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.println("Connected to :" + String(ssid));
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (mdns.begin(hostname, WiFi.localIP())) {
    Serial.println("MDNS responder started. Using \"" + String(hostname) + "\" as hostname.");
  }
  server.onNotFound(handleRequest);
  server.begin();
  Serial.println("HTTP server started.");
  delay(1000);
  switchSound(false);
}

void loop()
{
  server.handleClient();
  if (irrecv.decode(&results)) {
    setRecievedIr(&results);
    irrecv.resume();
  }
}
