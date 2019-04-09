#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define tx_pin 7
#define rx_pin 8

SoftwareSerial serial_test(rx_pin, tx_pin);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  serial_test.begin(9600);

  pinMode(7, OUTPUT);
  pinMode(8, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  DynamicJsonDocument jsonBuf(1024);
  DeserializationError err = deserializeJson(jsonBuf, serial_test);

  if(err){
    Serial.println("Failed to read data");
  }
  else{
    serializeJsonPretty(jsonBuf, Serial);
  }
}
