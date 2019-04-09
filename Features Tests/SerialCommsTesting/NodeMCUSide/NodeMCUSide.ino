#include <ArduinoJson.h>
#include <SoftwareSerial.h>

//char auth[] = "";
//
//char ssid[] = "Hmmmmm";
//char pass[] = "otnaille";

#define tx_pin D7
#define rx_pin D8

SoftwareSerial s(rx_pin, tx_pin);

bool stats = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//  Serial.swap();
  s.begin(9600);

  pinMode(rx_pin, INPUT);
  pinMode(tx_pin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  DynamicJsonDocument jsonBuffer(1024);
  jsonBuffer["led"] = stats;

  serializeJson(jsonBuffer, s);
  serializeJsonPretty(jsonBuffer, Serial);

//  if(s.available() > 0){
//    payload.printTo(s);
//    payload.printTo(Serial);
//    stats = !stats;
//  }
}

