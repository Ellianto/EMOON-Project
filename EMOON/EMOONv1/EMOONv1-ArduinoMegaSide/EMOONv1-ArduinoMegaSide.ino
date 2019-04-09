/*
  This Arduino Sketch is for the Arduino Mega Controller

  The Arduino Mega Controller will be receiving commands and data from
  the NodeMCU Side and manages actuators (motors, lights, displays).

  It will receive communication via Serial1 pins (D18 and D19) and 
  sends control signals corresponding to the received commands
*/

/* SerialCommunication Section
 * 
 * This part is omitted in the ArduinoMega since we use the TX1(D18) and RX1(D19)
 * for serial communication using Serial1. If another pin (digital) is to be used
 * for serial communication, these lines can be used to initiate a new Serial Interface
 *  
 *  #include <SoftwareSerial.h>
 *  #define tx_comms_pin 18
 *  #define rx_comms_pin 19
 *  SoftwareSerial toNodeMCU = SoftwareSerial(rx_comms_pin, tx_comms_pin);
 *  
 */

/* Arduino JSON Library
 * 
 * This section is for preparing the data received from NodeMCU
 * The data will be sent in JSON format, so we prepare a buffer
 * and a JSONObject to store the sent data.
 */
#include <ArduinoJson.h>
StaticJsonBuffer<2048> jsonBuffer;

/*
 * Stepper Motor Section
 */

#define curtains_in1  38
#define curtains_in2  39
#define curtains_in3  40
#define curtains_in4  41

void rollCurtain(bool isRaise){
  int Steps = 0;
  unsigned long last_time;
  unsigned long currentMillis ;
  int steps_left=4095;
  long time;

  while(steps_left>0){
    currentMillis = micros();
    if(currentMillis-last_time>=1000){

      //-------------------------------------------------
      for (int x=0;x<1;x++){
        switch(Steps){
           case 0:
             digitalWrite(curtains_in1, LOW); 
             digitalWrite(curtains_in2, LOW);
             digitalWrite(curtains_in3, LOW);
             digitalWrite(curtains_in4, HIGH);
           break; 
           case 1:
             digitalWrite(curtains_in1, LOW); 
             digitalWrite(curtains_in2, LOW);
             digitalWrite(curtains_in3, HIGH);
             digitalWrite(curtains_in4, HIGH);
           break; 
           case 2:
             digitalWrite(curtains_in1, LOW); 
             digitalWrite(curtains_in2, LOW);
             digitalWrite(curtains_in3, HIGH);
             digitalWrite(curtains_in4, LOW);
           break; 
           case 3:
             digitalWrite(curtains_in1, LOW); 
             digitalWrite(curtains_in2, HIGH);
             digitalWrite(curtains_in3, HIGH);
             digitalWrite(curtains_in4, LOW);
           break; 
           case 4:
             digitalWrite(curtains_in1, LOW); 
             digitalWrite(curtains_in2, HIGH);
             digitalWrite(curtains_in3, LOW);
             digitalWrite(curtains_in4, LOW);
           break; 
           case 5:
             digitalWrite(curtains_in1, HIGH); 
             digitalWrite(curtains_in2, HIGH);
             digitalWrite(curtains_in3, LOW);
             digitalWrite(curtains_in4, LOW);
           break; 
             case 6:
             digitalWrite(curtains_in1, HIGH); 
             digitalWrite(curtains_in2, LOW);
             digitalWrite(curtains_in3, LOW);
             digitalWrite(curtains_in4, LOW);
           break; 
           case 7:
             digitalWrite(curtains_in1, HIGH); 
             digitalWrite(curtains_in2, LOW);
             digitalWrite(curtains_in3, LOW);
             digitalWrite(curtains_in4, HIGH);
           break; 
           default:
             digitalWrite(curtains_in1, LOW); 
             digitalWrite(curtains_in2, LOW);
             digitalWrite(curtains_in3, LOW);
             digitalWrite(curtains_in4, LOW);
           break; 
        }
        
          if (isRaise) Steps++;
          else if(!isRaise) Steps--;
          
          if(Steps>7) Steps=0;
          else if(Steps<0) Steps=7;
      }
      //-------------------------------------------------
      
      time=time+micros()-last_time;
      last_time=micros();
      steps_left--;
    }
  }
}

/*
 * Servo Motor Section
 */
#include <Servo.h>
#define door_lock_pin 43
Servo door_lock;

void doorLock(){
  door_lock.write(90);
}

void doorUnlock(){
  door_lock.write(0);
}

/*
 * Other Actuator Pin Initializations
 */

#define fan_control_pin 42
#define main_light_pin  44
#define warm_light_pin  45
#define red_rgb_pin     46
#define green_rgb_pin   47
#define blue_rgb_pin    48

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600); //Standard Serial Monitor for Debugging
  Serial1.begin(9600); //Serial1 Interface for communicating with NodeMCU

  door_lock.attach(door_lock_pin);

  //Set pin modes
  pinMode(curtains_in1, OUTPUT);
  pinMode(curtains_in2, OUTPUT);
  pinMode(curtains_in3, OUTPUT);
  pinMode(curtains_in4, OUTPUT);
  pinMode(fan_control_pin, OUTPUT);
  pinMode(door_lock_pin, OUTPUT);
  pinMode(main_light_pin, OUTPUT);
  pinMode(warm_light_pin, OUTPUT);
  pinMode(red_rgb_pin, OUTPUT);
  pinMode(green_rgb_pin, OUTPUT);
  pinMode(blue_rgb_pin, OUTPUT);


  //Set initial values for all output pins
  digitalWrite(curtains_in1, LOW);
  digitalWrite(curtains_in2, LOW);
  digitalWrite(curtains_in3, LOW);
  digitalWrite(curtains_in4, LOW);
  digitalWrite(fan_control_pin, LOW);
  digitalWrite(door_lock_pin, LOW);
  digitalWrite(main_light_pin, LOW);
  digitalWrite(warm_light_pin, LOW);
  digitalWrite(red_rgb_pin, LOW);
  digitalWrite(green_rgb_pin, LOW);
  digitalWrite(blue_rgb_pin, LOW);
}


void loop() {
  // put your main code here, to run repeatedly:

  JsonObject& fromNodeMCU = jsonBuffer.parseObject(Serial1);
  
  if(fromNodeMCU == JsonObject::invalid()){
    //If data sent is invalid
    Serial.println("Invalid data format! Please recheck the connection or code!"); 
  }
  else{
    //Fan
    if(fromNodeMCU.containsKey("fan")){
      if(fromNodeMCU["fan"] == "on"){
        digitalWrite(fan_control_pin, HIGH);
        Serial.println("Fan turned on");
      }
      else if(fromNodeMCU["fan"] == "off"){
        digitalWrite(fan_control_pin, LOW);
        Serial.println("Fan turned off");
      }
    }
    
    //Servo Door Lock
    if(fromNodeMCU.containsKey("door_lock")){
      if(fromNodeMCU["door_lock"] == "lock"){
        doorLock();
      }
      else if(fromNodeMCU["door_lock"] == "unlock"){
        doorUnlock();
      }
    }
    
    //Stepper Motor Curtains
    if(fromNodeMCU.containsKey("curtains")){
      if(fromNodeMCU["curtains"] == "raise"){
        rollCurtain(true);
        Serial.println("Raising the curtain...");
      }
      else if(fromNodeMCU["curtains"] == "lower"){
        rollCurtain(false);
        Serial.println("Lowering the curtain...");
      }
    }
    
    //Main Lights
    if(fromNodeMCU.containsKey("main_lights")){
      if(fromNodeMCU["main_lights"] == "on"){
        digitalWrite(main_light_pin, HIGH);
        Serial.println("Main Lights turned on");
      }
      else if(fromNodeMCU["main_lights"] == "off"){
        digitalWrite(main_light_pin, LOW);
        Serial.println("Main Lights turned off");
      }
    }
    
    //RGB Lights
    if(fromNodeMCU.containsKey("RGB_lights")){
      if(fromNodeMCU["RGB_lights"]["red"] == "on"){
        digitalWrite(red_rgb_pin, HIGH);
        Serial.println("Red RGB turned on");
      }
      else if(fromNodeMCU["RGB_lights"]["red"] == "off"){
        digitalWrite(red_rgb_pin, LOW);
        Serial.println("Red RGB turned off");
      }

      if(fromNodeMCU["RGB_lights"]["green"] == "on"){
        digitalWrite(green_rgb_pin, HIGH);
        Serial.println("Green RGB turned on");
      }
      else if(fromNodeMCU["RGB_lights"]["green"] == "off"){
        digitalWrite(green_rgb_pin, LOW);
        Serial.println("Green RGB turned off");
      }

      if(fromNodeMCU["RGB_lights"]["blue"] == "on"){
        digitalWrite(blue_rgb_pin, HIGH);
        Serial.println("Blue RGB turned on");
      }
      else if(fromNodeMCU["RGB_lights"]["blue"] == "off"){
        digitalWrite(blue_rgb_pin, LOW);
        Serial.println("Blue RGB turned off");
      }
    }
    
    //Warm Lights
    if(fromNodeMCU.containsKey("warm_lights")){
      if(fromNodeMCU["warm_lights"] == "on"){
        digitalWrite(warm_light_pin, HIGH);
        Serial.println("Warm Lights turned on");
      }
      else if(fromNodeMCU["warm_lights"] == "off"){
        digitalWrite(warm_light_pin, LOW);
        Serial.println("Warm Lights turned off");
      }
    }
    
    //TFT Display
    if(fromNodeMCU.containsKey("monitor_display")){
      //Later implementation
      Serial.println("Display Data Received!");
    }
  }

  fromNodeMCU.prettyPrintTo(Serial);
  Serial.println(); 
}
