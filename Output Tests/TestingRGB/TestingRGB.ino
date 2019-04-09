#define R_light_pin D4
#define G_light_pin D3
#define B_light_pin D2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(R_light_pin, OUTPUT);
  pinMode(G_light_pin, OUTPUT);
  pinMode(B_light_pin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1500);
  digitalWrite(R_light_pin, HIGH);
  digitalWrite(G_light_pin, HIGH);
  digitalWrite(B_light_pin, HIGH);
  Serial.println("WHITE");

  delay(1500);
  digitalWrite(R_light_pin, LOW);
  digitalWrite(G_light_pin, HIGH);
  digitalWrite(B_light_pin, HIGH);
  Serial.println("NO RED");

  delay(1500);
  digitalWrite(R_light_pin, HIGH);
  digitalWrite(G_light_pin, LOW);
  digitalWrite(B_light_pin, HIGH);
  Serial.println("NO GREEN");

  delay(1500);
  digitalWrite(R_light_pin, HIGH);
  digitalWrite(G_light_pin, HIGH);
  digitalWrite(B_light_pin, LOW);
  Serial.println("NO BLUE");

    delay(1500);
  digitalWrite(R_light_pin, LOW);
  digitalWrite(G_light_pin, LOW);
  digitalWrite(B_light_pin, LOW);
  Serial.println("BLACKOUT");
}
