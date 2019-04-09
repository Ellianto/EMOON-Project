void setup() {
  Serial.begin(9600);
}
void loop() {
  //Output digital  : 0 = terang
  //                : 1 = gelap
  //Output analog   : Gede = gelap
  //                  Kecil = terang
  unsigned int AnalogValue = analogRead(A0);

  Serial.print("Analog Output : ");
  Serial.println(AnalogValue);

  unsigned int DigitalValue = digitalRead(5);

  Serial.print("Digital output :");
  Serial.println(DigitalValue);
  delay(500);
}
