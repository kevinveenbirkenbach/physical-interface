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
#include <NewRemoteReceiver.h>
#include <DHT.h>
#include "config.h"

/**
 * Define constants
 */
const uint16_t PIN_RADIO_TRANSMITTER = D7;
const uint16_t PIN_RADIO_RECIEVER = D8;
const uint16_t PIN_PIR  = D1;
const uint16_t PIN_DHT  = D6;
const uint16_t PIN_IR_RECIEVER = D4;
const uint16_t PIN_IR_SEND = D2;
const uint16_t PIN_LDR  = A0;
const uint16_t PIN_ACTIVE_BUZZER  = D5;
const uint16_t PIN_SOIL_MOISTURE = D3;

const char* PARAMETER_PLUG_ADDRESS="plug_address";
const char* PARAMETER_PLUG_ID="plug_id";
const char* PARAMETER_PLUG_STATUS="plug_enabled";
const char* PARAMETER_IR_TYPE="ir_type";
const char* PARAMETER_IR_CODE="ir_code";
const char* PARAMETER_IR_BITS="ir_bits";
const char* PARAMETER_PRE_DELAY_TIME_IN_MS="pre_delay_time_in_ms";
const char* PARAMETER_SOUND="sound_enabled";
const char* PARAMETER_LIST[]={PARAMETER_PLUG_ADDRESS,PARAMETER_PLUG_ID,PARAMETER_PLUG_STATUS,PARAMETER_IR_TYPE,PARAMETER_IR_CODE,PARAMETER_IR_BITS,PARAMETER_PRE_DELAY_TIME_IN_MS,PARAMETER_SOUND};

/**
 * Define variables
 */
decode_results results;
decode_type_t last_recieved_ir_type;
int last_recieved_ir_code;
int last_recieved_ir_bits;
unsigned int last_recieved_radio_period;
unsigned long last_recieved_radio_address;
unsigned long last_recieved_radio_groupBit;
unsigned long last_recieved_radio_unit;
unsigned long last_recieved_radio_switchType;

/**
 * Setup classes
 */
MDNSResponder mdns;
ESP8266WebServer server ( 80 );
IRrecv irrecv(PIN_IR_RECIEVER);


/**
 * Setter functions
 */
void setRecievedIr(decode_results *results) {
  last_recieved_ir_type = results->decode_type;
  last_recieved_ir_code = results->value;
  last_recieved_ir_bits = results->bits;
  Serial.println("The code \"" + String(last_recieved_ir_code) + "\" was send for type \"" + String(results->decode_type,DEC) + "\" with  \"" + String(results->bits,DEC) + "\" bits.");
}

void setLastRecievedRadio(unsigned int period, unsigned long address, unsigned long groupBit, unsigned long unit, unsigned long switchType){
    last_recieved_radio_period=period;
    last_recieved_radio_address=address;
    last_recieved_radio_groupBit=groupBit;
    last_recieved_radio_unit=unit;
    last_recieved_radio_switchType=switchType;
    Serial.println("Recieved radio code with the following values: address \"" + String(address)  + "\", period \"" + String(period)  + "\", unit \"" + String(unit)  + "\", group bit \"" + String(groupBit)  + "\" and switch type \"" + String(switchType) + "\".");
}

/**
 * Transmitter functions
 */
void sendRemoteGroupSignal(boolean state, NewRemoteTransmitter transmitter)
{
  transmitter.sendGroup(state);
  Serial.println("The state \"" + String(state,BIN) + "\" was send to the group.");
}

void sendRemoteUnitSignal(int unit, boolean state, NewRemoteTransmitter transmitter)
{
  transmitter.sendUnit(unit, state);
  Serial.println("The state \"" + String(state,BIN) + "\" was send to the switch \"" + String(unit,DEC) + "\".");
}

void sendIrCode(decode_type_t type,uint32_t code, uint16_t bits) {
  Serial.println("Enable IR-sender.");
  IRsend irsend(PIN_IR_SEND);
  irsend.begin();
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

  if(parameter==PARAMETER_PLUG_ADDRESS){
    return "long";
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
  if(isParameterDefined(PARAMETER_SOUND)){
    switchSound(server.arg(PARAMETER_SOUND).equals("on"));
  }
  if(isParameterDefined(PARAMETER_IR_TYPE) && isParameterDefined(PARAMETER_IR_CODE) && isParameterDefined(PARAMETER_IR_BITS)){
    sendIrCode(static_cast<decode_type_t>(server.arg(PARAMETER_IR_TYPE).toInt()),server.arg(PARAMETER_IR_CODE).toInt(),server.arg(PARAMETER_IR_BITS).toInt());
  }
  if(isParameterDefined(PARAMETER_PLUG_ID)){
    NewRemoteTransmitter transmitter(server.arg(PARAMETER_PLUG_ADDRESS).toInt(), PIN_RADIO_TRANSMITTER);
    if(server.arg(PARAMETER_PLUG_ID).equals("0")){
      sendRemoteGroupSignal(server.arg(PARAMETER_PLUG_STATUS).equals("on"),transmitter);
    }else if(server.arg(PARAMETER_PLUG_ID).toInt()>0){
      sendRemoteUnitSignal(server.arg(PARAMETER_PLUG_ID).toInt(),server.arg(PARAMETER_PLUG_STATUS).equals("on"),transmitter);
    }
  }
}

/**
 * Getter functions
 */
String getJsonDht(void){
  Serial.println("Reading DHT...");
  DHT dht(PIN_DHT, DHT11);
  delay(800); // Somehow this delay is needed to don't get "nan" values
  String temperature = String(dht.readTemperature());
  delay(800); // Somehow this delay is needed to don't get "nan" values
  String humidity    = String(dht.readHumidity());
  return "{\"temperature_celcius\":\""+ temperature +"\",\"relative_humidity\":\""+humidity+"\"}";
}

String getJsonRadio(void){
  Serial.println("Reading radio signal...");
  return "{\"last_recieved\":{\"period\":\""+String(last_recieved_radio_period)+"\",\"address\":\""+String(last_recieved_radio_address)+"\",\"group_bit\":\""+String(last_recieved_radio_groupBit)+"\",\"unit\":\""+String(last_recieved_radio_unit)+"\",\"switch_type\":\""+String(last_recieved_radio_switchType)+"\"}}";
}

String getJsonPir(void){
  Serial.println("Reading PIR...");
  pinMode(PIN_PIR, INPUT);
  return "{\"motion\":\""+String((digitalRead(PIN_PIR)==HIGH)?"true":"false")+"\"}";
}

String getJsonSoilMoisture(void){
  Serial.println("Reading soil moisture...");
  pinMode(PIN_SOIL_MOISTURE, INPUT);
  return "{\"is_moist\":\""+String((digitalRead(PIN_SOIL_MOISTURE)==LOW)?"true":"false")+"\"}";
}

String getJsonLdr(void){
  Serial.println("Reading LDR...");
  return "{\"actual\":\""+String(analogRead (PIN_LDR))+"\",\"minimum\":\"0\",\"maximum\":\"1023\"}";
}

String getJsonIr(void){
  Serial.println("Reading infared signal...");
  return "{\"last_recieved\":{\"bits\":\""+String(last_recieved_ir_bits)+"\",\"type\":\""+String(last_recieved_ir_type)+"\",\"data\":\""+String(last_recieved_ir_code)+"\"}}";
}

String getJson(void){
  Serial.println("Generating json...");
  return "{\"LDR\":"+String(getJsonLdr())+",\"DHT\":"+String(getJsonDht())+ ",\"soil_moisture\":"+String(getJsonSoilMoisture())+",\"PIR\":"+String(getJsonPir())+",\"IR\":"+String(getJsonIr())+",\"radio\":"+String(getJsonRadio())+"}";
}

#include "homepage_template.h"

void view(void){
  if(server.arg("format")=="json"){
    Serial.println("Json was called.");
    server.send ( 200, "text/html", getJson());
  }else{
    Serial.println("Html was called.");
    server.send ( 200, "application/json", homepage_template());
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
  Serial.begin(9600);
  Serial.println("Enable remote transmitter.");
  NewRemoteReceiver::init(PIN_RADIO_RECIEVER, 1, setLastRecievedRadio);
  Serial.println("Enable IR-reciever.");
  irrecv.enableIRIn();
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
    Serial.println("MDNS responder started.");
    Serial.println("Using \"" + String(hostname) + "\" as hostname.");
  }
  server.onNotFound(handleRequest);
  server.begin();
  Serial.println("HTTP server started.");
  Serial.println("Generate test sound.");
  switchSound(true);
  delay(200);
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
