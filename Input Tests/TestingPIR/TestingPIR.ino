///////////////////////code created by///////////////////////
//////////////////////KJ's electronics//////////////////////

int PIR_output = 7; // output of pir 

// With pins in front, from the left
// GND, Digital Out, Vcc

void setup() {
  pinMode(PIR_output, INPUT);// setting pir output as arduino input
  Serial.begin(9600);//serial communication between arduino and pc
}
void loop() {
  if(digitalRead(PIR_output) == HIGH) // reading the data from the pir sensor
   Serial.println("motion detected"); 
  
  else 
   Serial.println("scanning");
}
