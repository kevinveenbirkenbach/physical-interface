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

// Define Constants
const uint16_t pin_ritter = 13;
const uint16_t pin_pir  = 14;
const uint16_t pin_tmp  = 12;
const uint16_t pin_ir_reciever = 2;
const uint16_t pin_ir_send = 4;
const uint16_t pin_ldr  = A0;
const uint16_t pin_active_buzzer  = D5;
const unsigned long ritter_group_address = 13043702;
const char* parameter_plug_id="plug_id";
const char* parameter_plug_status="plug_status";
const char* parameter_ir_type="ir_type";
const char* parameter_ir_data="ir_code";
const char* parameter_ir_bits="ir_bits";
const char* parameter_delay_time_in_ms="delay_time_in_ms";
const char* parameter_sound_time_in_ms="sound_time_in_ms";
const char* parameter_list[]={parameter_plug_id,parameter_plug_status,parameter_ir_type,parameter_ir_data,parameter_ir_bits,parameter_delay_time_in_ms,parameter_sound_time_in_ms};

// Define variables
decode_results results;
decode_type_t last_recieved_ir_type;
int last_recieved_ir_data;
int last_recieved_ir_bits;

// Setup classes
MDNSResponder mdns;
ESP8266WebServer server ( 80 );
NewRemoteTransmitter transmitter(ritter_group_address, pin_ritter);
DHT dht(pin_tmp, DHT11);
IRrecv irrecv(pin_ir_reciever);
IRsend irsend(pin_ir_send);

void dump(decode_results *results) {
  last_recieved_ir_type = results->decode_type;
  last_recieved_ir_data = results->value;
  last_recieved_ir_bits = results->bits;
  Serial.println(getJsonIrLastRecieved());
}

// Switchs the whole group on
void setRitterGroup(int state)
{
  transmitter.sendGroup(state);
  Serial.println("The state \"" + String(state,BIN) + "\" was send to the group \"" + String(ritter_group_address,DEC) + "\".");
}

// Switchs one plug on
void setRitterSwitch(int unit, int state)
{
  transmitter.sendUnit(unit, state);
  Serial.println("The state \"" + String(state,BIN) + "\" was send to the switch \"" + String(unit,DEC) + "\".");
}

void setIrColor(decode_type_t type,uint32_t code, uint16_t bits) {
  irsend.send(type, code, bits);
  Serial.println("The code \"" + String(code) + "\" with \"" + String(bits) + "\" bits was send in format \"" + String(type) + "\".");
}

void setSound(int time_in_ms){
  Serial.println("Making sound for \"" + String(time_in_ms) + "ms.");
  pinMode(pin_active_buzzer,OUTPUT);
  digitalWrite(pin_active_buzzer,LOW);
  delay(time_in_ms);
  digitalWrite(pin_active_buzzer,HIGH);
  pinMode(pin_active_buzzer,INPUT);
}

bool isParameterDefined(String parameter_name){
  for (uint8_t parameter_index = 0; parameter_index < server.args(); parameter_index++) {
    if(server.argName(parameter_index)==parameter_name){
      return true;
    }
  }
  return false;
}

void controller(void){
  if(isParameterDefined(parameter_sound_time_in_ms)){
    setSound(server.arg(parameter_sound_time_in_ms).toInt());
  }
  if(isParameterDefined(parameter_ir_type) && isParameterDefined(parameter_ir_data) && isParameterDefined(parameter_ir_bits)){
    setIrColor(static_cast<decode_type_t>(server.arg(parameter_ir_type).toInt()),server.arg(parameter_ir_data).toInt(),server.arg(parameter_ir_bits).toInt());
  }
  if(isParameterDefined(parameter_plug_id) && isParameterDefined(parameter_plug_status)){
      if(server.arg(parameter_plug_id)=="group"){
        setRitterGroup(server.arg(parameter_plug_status).toInt());
      }else if(server.arg(parameter_plug_id).toInt()>0){
        setRitterSwitch(server.arg(parameter_plug_id).toInt(),server.arg(parameter_plug_status).toInt());
      }
  }
}


String getJsonDht(void){
  return "{\"temperature_celcius\":\""+String(dht.readTemperature())+"\",\"humidity\":\""+String(dht.readHumidity())+"\"}";
}

String getJsonPir(void){
  return "{\"motion\":\""+String(digitalRead(pin_pir))+"\"}";
}

String getJsonLdr(void){
  float volt = 5.0 /1024.0 * analogRead (pin_ldr);
  return "{\"input_volt\":\""+String(volt)+"\"}";
}

String getJsonIrLastRecieved(void){
  return "{\"last_recieved\":{\"bits\":\""+String(last_recieved_ir_bits)+"\",\"type\":\""+String(last_recieved_ir_type)+"\",\"data\":\""+String(last_recieved_ir_data)+"\"}}";
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
  if(isParameterDefined(parameter_delay_time_in_ms)){
      int delay_time_in_ms = server.arg(parameter_delay_time_in_ms).toInt();
      if(delay_time_in_ms>0){
        Serial.println("Applying delay time: " + server.arg(parameter_delay_time_in_ms) + "ms");
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
  pinMode(pin_pir, INPUT);
  Serial.println("Enable IR-reciever.");
  irrecv.enableIRIn();
  Serial.println("Enable IR-sender.");
  irsend.begin();
  Serial.println("Activate active buzzer.");
  setSound(1);
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
}

void loop()
{
  server.handleClient();
  if (irrecv.decode(&results)) {
    dump(&results);
    irrecv.resume();
  }
}
