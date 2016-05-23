
#include <Servo.h>
#include <Wire.h>
#include "Motor_Cmd.h"
#include "ADXL345lib.h"
#include "Odometer.h"

Accelerometer acc;
#define ACC_CHECK_IN  10
#define ACC_MOVING_TOLERANCE 0.1f
#define ACC_COLLISION_TOLERANCE 0.41f
long acc_time = 0;
bool acc_avail= false;
bool acc_moving = false;
bool acc_collision =false;
long acc_moving_elapsed =0;
long acc_moving_timeout = 2000;
double accX,accY,accZ;


Odometer odm;
#define ODM_SAMPLE_SIZE 20
#define ODM_CHECK_IN 25
#define ODM_PIN 0
#define ODM_TOLERANCE 10
#define ODM_TIRE_RADIUS 35


//commands
long currentTime =0;
#define MOTOR_CHECK_IN  500
long motor_time = 0;
bool halt =false;
bool movingForward = false;

void AccelerometerSetup(){
    accX = 0;accY = 0;accZ = 0;
    if (acc.begin(OSEPP_ACC_SW_ON) != 0)
    {
        Serial.println("Error connecting to accelerometer");
        acc_avail = false;
        return;
    }
    acc.setOffsets( -5,3,8);
    acc.setSensitivity(ADXL345_RANGE_PM2G);

    acc_time = millis();

    double x,y,z;
    if (acc.readGs(&x, &y, &z) != 0)
    {
        Serial.println("Failed to read accelerometer");
        acc_avail = false;
        return;
    }
    accX = x;
    accY = y;
    accZ = z;
    acc_avail = true;
}

void MotorSetup(){
    motor_time = millis();
    Serial.println(" enter '1' for forward");
    Serial.println(" enter '2' for backward");
    Serial.println(" enter '3' for stop");
    Serial.println(" enter '-10' or lower for right");
    Serial.println(" enter '10' or higher for left");
}


void ProcessMotor(long currTime)
{
  if((currTime-motor_time)> MOTOR_CHECK_IN)
  {
     while(Serial.available()>0)
     {
        int command = Serial.parseInt();
        Serial.print("command :");
        Serial.println(command);
        switch(command){
          case 1: Forward(FAST); break;
          case 2: Backward(FAST);break;
          case 3: halt = !halt;Stop();break;
         
         default:
          command = abs(command)>360?360:command;
          if(command<0){
            Turn(RIGHT,command);
          }else if (command>10){
            Turn(LEFT,command);
          }
          break;
        }
     }
      motor_time = currTime;
  }
}

void ProcessAccelData(long currTime)
{
    double x,y,z;
    if(acc_avail)
    {
      if( (currTime- acc_time)> ACC_CHECK_IN)
      {
        acc_moving = false;  
        if (acc.readGs(&x, &y, &z) != 0)
        {
            Serial.println("Failed to read accelerometer");
            return;
        }
        
        if( abs((abs(accY)- abs(y))) >= ACC_MOVING_TOLERANCE){ 
          acc_moving = true;  
          Serial.println("Acc moving Y");
        }
         if( abs((abs(accZ)- abs(z))) >= ACC_MOVING_TOLERANCE){ 
          acc_moving = true;  
          Serial.println("Acc moving Z");
        }
        if( abs((abs(accX)- abs(x))) >= ACC_MOVING_TOLERANCE){ 
          acc_moving = true;  
          Serial.println("Acc moving X");
        }

       
        if(abs((abs(accY)- abs(y))) >= ACC_COLLISION_TOLERANCE
        || abs((abs(accX)- abs(x))) >= ACC_COLLISION_TOLERANCE
        || abs((abs(accZ)- abs(z))) >= ACC_COLLISION_TOLERANCE){ 
          acc_collision = true;  
           Serial.println("Acc Front/Back End Collision");
        }
         
        acc_moving_elapsed   = acc_moving? currTime:acc_moving_elapsed;
        acc_moving  = acc_moving || (currTime -acc_moving_elapsed)< acc_moving_timeout;

        accZ = z;
        accY = y;
        accX = x;
        acc_time = currTime;
      }
    }
}

void resetNavigation(long currTime)
{
  Forward(HALF_SPEED);
  acc_moving_elapsed = currTime;
  odm.resetValues(currTime);
  acc_time = currTime;
  acc_collision = false;
  acc_moving_elapsed  = currTime;
  movingForward = true;
}


void setup() {
  Serial.begin(9600);
  currentTime = millis();
  AccelerometerSetup();
  MotorSetup();
  odm.setup( ODM_SAMPLE_SIZE, ODM_CHECK_IN,ODM_TIRE_RADIUS, ODM_PIN,ODM_TOLERANCE);
  resetNavigation(currentTime);
}

void loop() {
  long currTime = millis();
  odm.update(currTime);
  ProcessMotor(currTime);
  ProcessAccelData(currTime);
  if(halt){
    return;
  }


if(!movingForward && acc_moving && odm.isMoving())
{ 
    Serial.println("got stuck, moved backwards, now turning to avoid the obstacle");
    Turn(RIGHT,20);
    resetNavigation(millis());
}

/*
 //untested
if(!movingForward && !acc_moving && !odm.isMoving())
{
   Serial.print ("going forward fast");
  //pick a random direction to move
   movingForward = true;
  Forward(FAST);
  delay(1000);
}else{
  Serial.print ("acc moving ?");
  Serial.print (acc_moving);
  Serial.print ("  odm moving ?");
  Serial.println (odm.isMoving());
  
}
*/


 //tested and used
if(acc_collision)
{
  //begin evasive manuevers
  //Are we turning?
  acc_collision = false;
  Serial.println("collided turning left");
  Turn(RIGHT,20);
  Stop();
  resetNavigation(currTime);
}

 
  
  if( !acc_moving || !odm.isMoving() )
  {
    Serial.println("acc_moving = false backgin up ");
    Turn(RIGHT,30);
    resetNavigation(currTime);
  }
  /*
  Serial.println("stuck turning right");
  //we are stuck or hit something
    acc_collision = false;
    acc_moving = false;
    Backward(HALF_SPEED);
    delay(3000);
    Turn(RIGHT,40);
    Stop();
    movingForward = false;
}

*/
  
}



