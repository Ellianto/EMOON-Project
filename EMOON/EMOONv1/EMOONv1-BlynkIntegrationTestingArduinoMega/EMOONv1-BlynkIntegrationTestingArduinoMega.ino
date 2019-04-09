#include <BlynkSimpleStream.h>
#include <Servo.h>
Servo door_lock;

#define red_rgb_pin     24
#define green_rgb_pin   25
#define blue_rgb_pin    26
#define warm_light_pin  27
#define main_light_pin  28

#define curtains_in1  34
#define curtains_in2  35
#define curtains_in3  36
#define curtains_in4  37

#define fan_control_pin 42
#define door_lock_pin   43

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "3950f6d46c794b939c2b0740c49d50dd";

void setup()
{
  // Blynk will work through Serial
  // Do not read or write this serial manually in your sketch
  Serial.begin(9600);
  Blynk.begin(Serial, auth);

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

  door_lock.write(0);
}

void loop()
{
  // Curtain Rolling masih serial, mungkin butuh update supaya paralel
  // Percobaan pakai flag global gagal, mungkin harus pakai timer
  Blynk.run();
}

// Stepper

// Servo
void doorLock(){
  door_lock.write(90);
}

void doorUnlock(){
  door_lock.write(0);
}

//RGB Lights, butuh PWM
void rgbControl(bool redOn, bool greenOn, bool blueOn){
  if(redOn) digitalWrite(red_rgb_pin, HIGH);
  else digitalWrite(red_rgb_pin, LOW);

  if(greenOn) digitalWrite(green_rgb_pin, HIGH);
  else digitalWrite(green_rgb_pin, LOW);

  if(blueOn) digitalWrite(blue_rgb_pin, HIGH);
  else digitalWrite(blue_rgb_pin, LOW);
}

// Curtains Control
BLYNK_WRITE(V0){
  int curtains = param.asInt(); // Get values as Integer

  if(curtains == 1)
    rollCurtain(true);
  else if (curtains == 0)
    rollCurtain(false);  
}

// Fan Control
BLYNK_WRITE(V1){
  int fans = param.asInt();

  if(fans == 1)
    digitalWrite(fan_control_pin, HIGH);
  else if (fans == 0)
    digitalWrite(fan_control_pin, LOW);
}

// Door Lock Control
BLYNK_WRITE(V2){
  int door_lock = param.asInt();

  if(door_lock == 1)
    doorLock();
  else if (door_lock == 0)
    doorUnlock();
}

// Main Lights Control
BLYNK_WRITE(V3){
  int main_light = param.asInt();

  if(main_light == 1)
    digitalWrite(main_light_pin, HIGH);
  else if(main_light == 0)
    digitalWrite(main_light_pin, LOW);
}

// Warm Lights Control
BLYNK_WRITE(V4){
  int warm_lights = param.asInt();
  
  if(warm_lights == 1)
    digitalWrite(warm_light_pin, HIGH);
  else if (warm_lights == 0)
    digitalWrite(warm_light_pin, LOW);
}

// RGB Lights Control
BLYNK_WRITE(V5){
  int rgb_lights = param.asInt();

  switch(rgb_lights){
    case 1:{ //White
      rgbControl(true, true, true);
      break;
    }
    case 2:{ //Red
      rgbControl(true, false, false);
      break;
    }
    case 3:{ //Green
      rgbControl(false, true, false);
      break;
    }
    case 4:{ //Blue
      rgbControl(false, false, true);
      break;
    }
    case 5:{ //Off
      rgbControl(false, false, false);
      break;
    }
  }
}

