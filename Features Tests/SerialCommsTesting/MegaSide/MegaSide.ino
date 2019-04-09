#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define tx_comms_pin 52
#define rx_comms_pin 53

#define led_pin 13

#define red_pin A8
#define green_pin A9
#define blue_pin A10

//SoftwareSerial s(rx_comms_pin, tx_comms_pin);

void setup() {
  // put your setup code here, to run once:
//  pinMode(red_pin, OUTPUT);
//  pinMode(green_pin, OUTPUT);
//  pinMode(blue_pin, OUTPUT);
    pinMode(led_pin, OUTPUT);
  
    Serial.begin(9600);
    Serial1.begin(9600);
//    s.begin(9600);
}

void loop(){

  //Check for incoming data

    StaticJsonBuffer<1000> jsonBuffer;
    JsonObject& result = jsonBuffer.parseObject(Serial1);

    if(result == JsonObject::invalid()){
      Serial.println("Failed to read JSON");
    }
    else{
      result.prettyPrintTo(Serial);
      if(result["led"]){
        digitalWrite(led_pin, HIGH);
        Serial.println("turning LED on");
      }
      else{
        digitalWrite(led_pin, LOW);
        Serial.println("turning LED off");
      }
    }
}

/*
void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  analogWrite(red_pin, 1023);
  analogWrite(green_pin, 1023);
  analogWrite(blue_pin, 1023);
  Serial.println("White Full Brightness");

  delay(2000);
  //analogWrite(red_pin, 1023);
  //analogWrite(green_pin, 1023);
  analogWrite(blue_pin, 1023);
  Serial.println("Blue Full Brightness");

  delay(2000);
  //analogWrite(red_pin, 1023);
  analogWrite(green_pin, 1023);
  //analogWrite(blue_pin, 1023);
  Serial.println("Green Full Brightness");

  delay(2000);
  analogWrite(red_pin, 1023);
  //analogWrite(green_pin, 1023);
  //analogWrite(blue_pin, 1023);
  Serial.println("White Full Brightness");
}
*/
