#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


struct nrfdata{
  byte J1X,J2Y;
}data;
RF24 radio(9, 10);
const byte address[6] = "00001";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  // put your main code here, to run repeatedly:
  radio.read(&data,sizeof(struct nrfdata));
  float mov = 0;
    if(data.J1X > 150){
      mov = map(long(data.J1X),150.0,255.0,0.0,3.0);
    }
    else if(data.J1X < 100){
      mov = map(long(data.J1X),0.0,100.0,-3.0,0.0);
    }
    else{
      mov = 0.0;
    }
  Serial.print("X :");
  Serial.print(mov);
  float rot = 0;
    if(data.J2Y > 150){
      rot = map(long(data.J2Y),150.0,255.0,0.0,50.0);
    }
    else if(data.J2Y < 100){
      rot = map(long(data.J2Y),0.0,100.0,-50.0,0.0);
    }
    else{
      rot = 0;
    }
  Serial.print(" Y :");
  Serial.print(rot);
  Serial.print("\n");
}
