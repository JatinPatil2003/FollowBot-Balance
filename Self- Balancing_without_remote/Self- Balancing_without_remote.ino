#include "I2Cdev.h"
#include <PID_v1.h>
#include "MPU6050_6Axis_MotionApps20.h"
MPU6050 mpu;
#define minspeed 75

bool dmpReady = false;  
uint8_t mpuIntStatus;    
uint8_t devStatus;       
uint16_t packetSize;     
uint16_t fifoCount;      
uint8_t fifoBuffer[64];  
 
Quaternion q;            
VectorFloat gravity;     
float ypr[3];   
double nowtime=0;
double prevtime=0;        
 
double setpoint = 184.2;//184.2
double Kp = 10.5; //21//30//17.5
double Kd = 0.3937; //0.75 
double Ki = 70;  //20

double input, output;
//double set = 184.2;
PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);
int low = 150;
int high = 210;


volatile bool mpuInterrupt = false;      
void dmpDataReady()
{
  mpuInterrupt = true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
//  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(2, INPUT);

//   Serial.println(F("Testing device connections..."));
//  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

   devStatus = mpu.dmpInitialize();


 // offsets  -1966 -1101 1139  690 -20 -19

//  Data is prited as: acelX acelY aclZ giro iroY giroZ

  mpu.setXGyroOffset(690);
  mpu.setYGyroOffset(-20);
  mpu.setZGyroOffset(-19);
  mpu.setZAccelOffset(1139);
 
  if (devStatus == 0)
  { 
//    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);
 
//    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

//     Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

     packetSize = mpu.dmpGetFIFOPacketSize();

     pid.SetMode(AUTOMATIC);
    pid.SetSampleTime(10);
    pid.SetOutputLimits(-255+minspeed, 255-minspeed);
  }
  else
  { 
//    Serial.print(F("DMP Initialization failed (code "));
//    Serial.print(devStatus);
//    Serial.println(F(")"));
  }

   pinMode (3, OUTPUT);
  pinMode (4, OUTPUT);
  pinMode (5, OUTPUT);
  pinMode (6, OUTPUT);
  pinMode (7, OUTPUT);
  pinMode (8, OUTPUT);

   digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  analogWrite(5,0);
  analogWrite(6,0);
}



void loop() {

   if (!dmpReady) return;

   while (!mpuInterrupt && fifoCount < packetSize)
  {
     pid.Compute();

//     Serial.print(low);
//     Serial.print(" ");
//     Serial.print(high);
//     Serial.print(" ");
     Serial.print(input);
     Serial.print(" ");
     nowtime = millis();
     Serial.print(nowtime-prevtime);
     prevtime = nowtime;
     Serial.print(" ");
     Serial.print(setpoint); 
     Serial.print(" ");
     Serial.println(output);

    // input -= 180;


    if (input > 150 && input < 210) {  
      if (output > 0) 
        Forward();  
      else if (output < 0)  
        Backward();  
    }
    else  
      Stop();  

  }

   mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

   fifoCount = mpu.getFIFOCount();

   if ((mpuIntStatus & 0x10) || fifoCount == 1024)
  {
     mpu.resetFIFO();
//    Serial.println(F("FIFO overflow!"));

   }
  else if (mpuIntStatus & 0x02)
  {
     while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

     mpu.getFIFOBytes(fifoBuffer, packetSize);

     fifoCount -= packetSize;

    mpu.dmpGetQuaternion(&q, fifoBuffer);  
    mpu.dmpGetGravity(&gravity, &q);  
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity); 

    input = ypr[1] * 180 / M_PI + 180;
     
  }
}

void Forward()  
{
  if(abs(setpoint-input)<=0){
    analogWrite(5,0);
    analogWrite(6,0);
  }
  else{
    analogWrite(5,constrain(output+minspeed,minspeed,255));
    analogWrite(6,constrain(output+minspeed,minspeed,255));
  }
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  
}

void Backward()  
{
  if(abs(setpoint-input)<=0){
    analogWrite(5,0);
    analogWrite(6,0);
  }
  else{
    analogWrite(5,constrain(minspeed+(output*-1),minspeed,255));
    analogWrite(6,constrain(minspeed+(output*-1),minspeed,255));
  }
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(7, LOW);
}

void Stop()   
{
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  analogWrite(5,0);
  analogWrite(6,0);
}
