void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  Serial.begin(9600);
}

int value = 0;

void loop() {
//  Digital Switching
//  delay(1000);
//  Serial.println("Turning On!");
//  digitalWrite(7, HIGH);
//
//  delay(1000);
//  Serial.println("Turning Off!");
//  digitalWrite(7, LOW);

//    Analog Switching, same result as digital
//    delay(200);
//    Serial.print("Value : ");
//    Serial.println(value);
//    analogWrite(A3, value);
//    value = value + 255;
//
//    if(value > 1024) value = 0;

//  RGB Testing

    delay(1000);
    Serial.println("WHITE");
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);

    delay(1000);
    Serial.println("RED");
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);

    delay(1000);
    Serial.println("GREEN");
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);

    delay(1000);
    Serial.println("BLUE");
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);

}
