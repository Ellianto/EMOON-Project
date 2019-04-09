#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "07c853e56e2d4c40b8eb50e5f3caf101";

char ssid[] = "Hmmmm";
char pass[] = "otnaille";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

BLYNK_WRITE(V2){
  int door_lock = param.asInt();

  if(door_lock == 1)
    Serial.println("OPEN SESAME");
  else if (door_lock == 0)
    Serial.println("CLOSE SESAME");
}

void loop() {
  // put your main code here, to run repeatedly:
  //Blynk.run();
  digitalWrite(D2, HIGH);
}
