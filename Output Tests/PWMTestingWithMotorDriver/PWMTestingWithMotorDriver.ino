void setup(){
  Serial.begin(9600);

  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(7,OUTPUT);
}

void loop(){
  digitalWrite(7, HIGH);
  digitalWrite(11, LOW);
  digitalWrite(10, HIGH);

  delay(1000);
  analogWrite(9, 64);

  delay(1000);
  analogWrite(9,255);
}

//#define enB 9 // Ke Pin PWM
//#define in3 6
//#define in4 7
//
//void setup() {
//  Serial.begin(9600);
//  pinMode(enB, OUTPUT);
//  pinMode(in3, OUTPUT);
//  pinMode(in4, OUTPUT);
//  digitalWrite(in3, HIGH);
//  digitalWrite(in4, LOW);
//}
//void loop() {
//  delay(1500);
//  Serial.println("Testing");
//  analogWrite(enB, 64);
//
//  delay(1500);
//  Serial.println("Coba-coba");
//  analogWrite(enB, 255);
//}
//
////void setup() {
////  // put your setup code here, to run once:
////  pinMode(6, OUTPUT);
////  Serial.begin(9600);
////}
////
////void loop() {
////  // put your main code here, to run repeatedly:
////  analogWrite(6, 0);
////  Serial.println("Mati");
////  delay(1000);
////
////  analogWrite(6, 64);
////  Serial.println("Setengah");
////  delay(1000);
////
////  analogWrite(6, 255);
////  Serial.println("Full");
////  delay(1000);
////}
