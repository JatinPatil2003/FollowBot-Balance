#include "I2Cdev.h"
#include <PID_v1.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define minspeed 75

struct nrfdata{
  byte J1X,J2Y;
}data;
RF24 radio(9, 10);
const byte address[6] = "00001";

MPU6050 mpu;

// MPU control/status variables
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion variables
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


//............set following 4 values for your robot....
double setpoint = 184.2;
double Kp = 10.5; //Set this first
double Kd = 0.3937; //Set this secound
double Ki = 70; //Finally set this
//..............................................

double input, output;
PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);



volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
  mpuInterrupt = true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(2, INPUT);

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // load and configure the DMP
  devStatus = mpu.dmpInitialize();


  // supply your own gyro offsets here, scaled for min sensitivity
//  Your offsets  -1966 -1101 1139  690 -20 -19
//
//  Data is prited as: acelX acelY aclZ giro iroY giroZ

  mpu.setXGyroOffset(690);
  mpu.setYGyroOffset(-20);
  mpu.setZGyroOffset(-19);
  mpu.setZAccelOffset(1139);

  // make sure it worked (returns 0 if so)
  if (devStatus == 0)
  {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();

    //setup PID
    pid.SetMode(AUTOMATIC);
    pid.SetSampleTime(10);
    pid.SetOutputLimits(-180, 180);
  }
  else
  {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }

  //Initialise the Motor output pins
  pinMode (3, OUTPUT);
  pinMode (4, OUTPUT);
  pinMode (5, OUTPUT);
  pinMode (6, OUTPUT);
  pinMode (7, OUTPUT);
  pinMode (8, OUTPUT);

  //By default turn off both the motors
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  analogWrite(5,0);
  analogWrite(6,0);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}



void loop() {

  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize)
  {
    //no mpu data - performing PID calculations and output to motors
    data.J1X = 128;
    data.J2Y = 128;
    
    radio.read(&data,sizeof(struct nrfdata));
    int mov = 0;
    if(data.J1X > 150){
      mov = map(data.J1X,150,255,0,3);
    }
    else if(data.J1X < 100){
      mov = map(data.J1X,0,100,-3,0);
    }
    else{
      mov = 0;
    }
    setpoint += mov;
//    Serial.print(mov);Serial.print(" || ");
    pid.Compute();

    //Print the value of Input and Output on serial monitor to check how it is working.
    Serial.print(input); Serial.print(" =>"); Serial.println(output);
    
    int rot = 0;
    if(data.J2Y > 150){
      rot = map(data.J2Y,150,255,0,50);
    }
    else if(data.J2Y < 100){
      rot = map(data.J2Y,0,100,-50,0);
    }
    else{
      rot = 0;
    }
//    Serial.print(" || ");Serial.println(rot);
    if (input > 150 && input < 200) { //If the Bot is falling

      if (output > 0) //Falling towards front
        Forward(rot); //Rotate the wheels forward
      else if (output < 0) //Falling towards back
        Backward(rot); //Rotate the wheels backward
    }
    else //If Bot not falling
      Stop(); //Hold the wheels still

    setpoint -= mov;

  }

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024)
  {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  }
  else if (mpuIntStatus & 0x02)
  {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;

    mpu.dmpGetQuaternion(&q, fifoBuffer); 
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity); 

    input = ypr[1] * 180 / M_PI + 180;
    //        Serial.println("ypr[1]=");
    //Serial.println(ypr[1]);
  }
}

void Forward(int con) //Rotate the wheel forward
{
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  analogWrite(5,constrain(output+con+minspeed,minspeed,255));
  analogWrite(6,constrain(output-con+minspeed,minspeed,255));
}

void Backward(int con) //Rotate the wheel Backward
{
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(7, LOW);
  analogWrite(5,constrain((output*-1)-con+minspeed,minspeed,255));
  analogWrite(6,constrain((output*-1)+con+minspeed,minspeed,255));
}

void Stop() //Stop both the wheels
{
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  analogWrite(5,0);
  analogWrite(6,0);
}
