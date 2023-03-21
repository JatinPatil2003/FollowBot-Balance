#define switchPin1 3
#define switchPin2 4
#define switchPin3 5
#define switchPin4 6
#define potPin1 A7
#define potPin2 A6
#define joyPin1_1 A2
#define joyPin1_2 A3
#define joyPin2_1 A0
#define joyPin2_2 A1
struct nrfdata{
  int S1,S2,S3,S4;
  int Pot1,Pot2;
  int J1X,J1Y,J2X,J2Y;
}data;


void setup() {
  // put your setup code here, to run once:
  pinMode(switchPin1,INPUT);
  pinMode(switchPin2,INPUT);
  pinMode(switchPin3,INPUT);
  pinMode(switchPin4,INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  data.S1 = digitalRead(switchPin1);
  data.S2 = digitalRead(switchPin2);
  data.S3 = digitalRead(switchPin3);
  data.S4 = digitalRead(switchPin4);
  data.Pot1 = analogRead(potPin1);
  data.Pot2 = analogRead(potPin2);
  data.J1X = analogRead(joyPin1_1);
  data.J1Y = analogRead(joyPin1_2);
  data.J2X = analogRead(joyPin2_1);
  data.J2Y = analogRead(joyPin2_2);
  delay(100);
  Serial.print("----------------------------------------\n");
  Serial.print(" | ");
  Serial.print(data.J1X);
  Serial.print(" | ");
  Serial.print(data.J1Y);
  Serial.print(" | ");
  Serial.print(data.J2X);
  Serial.print(" | ");
  Serial.print(data.J2Y);
  Serial.print(" | ");
  Serial.print(data.Pot1);
  Serial.print(" | ");
  Serial.print(data.Pot2);
  Serial.print(" | ");
  Serial.print(data.S1);
  Serial.print(" | ");
  Serial.print(data.S2);
  Serial.print(" | ");
  Serial.print(data.S3);
  Serial.print(" | ");
  Serial.print(data.S4);
  Serial.print("\n");
}
