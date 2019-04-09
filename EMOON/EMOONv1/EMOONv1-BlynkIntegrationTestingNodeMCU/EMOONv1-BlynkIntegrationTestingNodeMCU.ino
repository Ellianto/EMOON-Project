/*
  This Arduino Sketch is for the NodeMCU Controller

  The NodeMCU will be communicating with the handphone through the Blynk App,
  and takes commands from the user through the Blynk App, and then will pass
  the command to the Arduino Mega Controller.

  The commands will be sent using JSON, through the secondary Serial interface
  (D7 and D8) by triggering Serial.swap();

  The NodeMCU here will act as a station
*/

//#include <ArduinoJson.h>
//#include <SoftwareSerial.h>
//SoftwareSerial Serial = SoftwareSerial(9,10);

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial

// Auth Token from Elli's Phone
char auth[] = "07c853e56e2d4c40b8eb50e5f3caf101";

// Connection Problem with certain WiFi, use phone HotSpot for now
// Set password to "" for open networks

// For now, needs to use phone's HotSpot WiFi to work
char ssid[] = "Hmmmm";
char pass[] = "otnaille";

BlynkTimer timer; // Create a Timer object called "timer"! 

//Virtual pins definition
//#define V10 temp_virtual_pin
//#define V11 humid_virtual_pin
//#define V12 brightness_virtual_pin
//#define V13 gas_virtual_pin

//Physical pins definition
#define ldr_pin 5   //D1 = GPIO5
#define gas_pin 4   //D2 = GPIO4
#define dht22_pin 0 //D3 = GPIO0
#define pir_pin 2   //D4 = GPIO2

#include "DHT.h"
#define DHTTYPE DHT22
DHT dht22(dht22_pin, DHTTYPE);

WidgetTerminal pir_log(V14);

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sendUptime); //  Here you set interval (1sec) and which function to call 
  dht22.begin();
  pir_log.clear();
}

float parseTemperature(){
  return dht22.computeHeatIndex(dht22.readTemperature(), dht22.readHumidity(), false);
}

float parseHumidity(){
  return dht22.readHumidity();
}

String parseGas(){
  int gas_concentration = digitalRead(gas_pin);
  if(gas_concentration == 1)
    return "Normal";
  else if (gas_concentration == 0)
    return "High";
  else return "ERROR";
}

String parseBrightness(){
  int isDim = digitalRead(ldr_pin);
  if(isDim == 1)
    return "Dim";
  else if(isDim == 0)
    return "Bright";
  else return "ERROR";
}

void logMovement(){
  if(digitalRead(pir_pin) == 1){
    Serial.println("Movement Detected!");
    pir_log.println("Movement Detected!");
    pir_log.flush();
  }
  else Serial.println("Standby...");
}

void sendUptime()
{
  // This function sends Arduino up time every 1 second to Virtual Pin (V5)
  // In the app, Widget's reading frequency should be set to PUSH
  // You can send anything with any interval using this construction
  // Don't send more that 10 values per second

  // Blynk.virtualWrite(V5, millis() / 1000);

  Blynk.virtualWrite(V10, parseTemperature());
  Blynk.virtualWrite(V11, parseHumidity());
  Blynk.virtualWrite(V12, parseBrightness());
  Blynk.virtualWrite(V13, parseGas());
  logMovement();

  Serial.print("Temperature : ");
  Serial.print(parseTemperature());
  Serial.println(" oC");

  Serial.print("Humidity : ");
  Serial.print(parseHumidity());
  Serial.println(" RH");

  Serial.print("Brightness : ");
  Serial.println(parseBrightness());

  Serial.print("Natural Gas Concentration : ");
  Serial.println(parseGas());
  Serial.println();
}

void loop()
{
  Blynk.run(); // all the Blynk magic happens here
  timer.run(); // BlynkTimer is working...
}
