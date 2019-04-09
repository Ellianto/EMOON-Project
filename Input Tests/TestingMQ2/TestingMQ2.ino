/*******
 
 All the resources for this project:
 https://www.hackster.io/Aritro

*******/
int smokeDigital = 5; //D0 = GPIO5
int smokeAnalog = A0;
// Your threshold value
//int sensorThres = 400;

//Output digital  : 1 = normal
//                  0 = high concentration of gas
//Output analog   : Tinggi = high concentration of gas
//                  Rendah = normal

void setup() {
  pinMode(smokeDigital, INPUT);
  pinMode(smokeAnalog, INPUT);
  Serial.begin(9600);
}

void loop() {
  int analogSensor = analogRead(smokeAnalog);
  int digitalSensor = digitalRead(smokeDigital);

  Serial.print("Pin Analog : ");
  Serial.println(analogSensor);

  Serial.print("Pin Digital : ");
  Serial.println(digitalSensor);
  
  delay(100);
}
