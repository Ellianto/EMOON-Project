#include <TimeLib.h>

void setup(){
  Serial.begin(9600);
}

void loop(){
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());

  Serial.print(" ");
  Serial.print(weekday());
  Serial.print(", ");
  Serial.print(day());
  Serial.print("-");
  Serial.print(month());
  Serial.print("-");
  Serial.print(year());
  Serial.println();
}

