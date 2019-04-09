/*
  This Arduino Sketch is for the NodeMCU Controller

  The NodeMCU will be communicating with the handphone through the Blynk App,
  and takes commands from the user through the Blynk App, and then will pass
  the command to the Arduino Mega Controller.

  The commands will be sent using JSON, through the secondary Serial interface
  (D7 and D8) by triggering Serial.swap();

  The NodeMCU here will act as a station
*/

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoJson.h>

#include <SoftwareSerial.h>
#define tx_comms_pin D8
#define rx_comms_pin D7

SoftwareSerial s = SoftwareSerial(rx_comms_pin, tx_comms_pin);
//#define BLYNK_PRINT Serial

// Auth Token from Elli's Phone
char auth[] = "07c853e56e2d4c40b8eb50e5f3caf101";

// Connection Problem with certain WiFi, use phone HotSpot for now
// Set password to "" for open networks
char ssid[] = "Hmmmm";
char pass[] = "otnaille";

void setup()
{
  // Debug console
  Serial.begin(9600);
  Serial.swap();
  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run(); 
}

StaticJsonBuffer<2048> jsonBuffer;

void sendToMega(char key[], char value[]){
  delay(500);
  JsonObject& payload = jsonBuffer.createObject();

  payload[key] = value;

  //payload.prettyPrintTo(Serial);

  if(Serial.available()){
    payload.printTo(Serial);
  }
}

BLYNK_WRITE(V0){
  delay(100);
  int curtains = param.asInt(); // Get values as Integer

  if(curtains == 1)
    sendToMega("curtains", "raise");
  else if (curtains == 0)
    sendToMega("curtains", "lower"); 
}

BLYNK_WRITE(V1){
  int fans = param.asInt();

  if(fans == 1)
    sendToMega("fan", "on");

  else if (fans == 0)
    sendToMega("fan", "off");
}

BLYNK_WRITE(V2){
  int door_lock = param.asInt();

  if(door_lock == 1)
    sendToMega("door_lock", "lock");
  
  else if (door_lock == 0)
    sendToMega("door_lock", "unlock");
}

