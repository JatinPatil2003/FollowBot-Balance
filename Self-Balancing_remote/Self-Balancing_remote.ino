#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define joyPin1_1 A0
#define joyPin2_2 A3
#define switchPin1 3
#define switchPin2 4
#define potPin1 A7
#define potPin2 A6
struct nrfdata{
  byte J1X,J2Y;
}data;
RF24 radio(9, 10);
const byte address[6] = "00001";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(switchPin1,INPUT);
  pinMode(switchPin2,INPUT);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

//  RX----------------------------
//  Serial.begin(9600);
//  radio.begin();
//  radio.openReadingPipe(0, address);
//  radio.setPALevel(RF24_PA_MIN);
//  radio.startListening();
}

void loop() {
  // put your main code here, to run repeatedly:
  int S1 = digitalRead(switchPin1);
  int S2 = digitalRead(switchPin2);
  if(S1 == 1 && S2 == 0){
    int X = analogRead(joyPin1_1);
    int Y = analogRead(joyPin2_2);
    data.J1X = map(X,0,1023,0,255);
    data.J2Y = map(Y,0,1023,0,255);
  }
  else if(S2 == 1 && S1 == 0){
    int X = analogRead(potPin1);
    int Y = analogRead(potPin2);
    data.J1X = map(X,0,1023,0,255);
    data.J2Y = map(Y,0,1023,0,255);
  }
  radio.write(&data,sizeof(struct nrfdata));
  
  

//  RX-------------------------------------
//  radio.read(data,sizeof(struct nrfdata));
}
