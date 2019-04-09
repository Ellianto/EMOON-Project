void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(7,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  digitalWrite(7, HIGH);
  Serial.println("ACTIVE");

  delay(2000);
  digitalWrite(7,LOW);
  Serial.println("LOW");
}
