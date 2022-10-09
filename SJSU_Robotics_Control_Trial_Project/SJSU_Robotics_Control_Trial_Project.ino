#include <Wire.h>
#include <Servo.h>

//declared I2C address for accelerometer
const int MPU_addr = 0x68;
//read values from wire
int16_t wr_X,wr_Y,wr_Z;
//angle values
double x;
double y;
double z;

//possible minimum and maximum wire.read values
int minVal=265;
int maxVal=402;

int servoPin = 9;
int servoPos = 90;
Servo myServo;

//can switch forX and forY to false/true based on axis of rotation
//true = axis of rotation
bool forX = true;
bool forY = false;

//difference gets QI angle values
//angle_offset adjusts to relevant quadrant movement
float difference;
float angle_offset;

//change origin value to '-' for positive angle value, '+' degree for negative angle value 
float origin = 0.00;

//variable adds angle offset and origin (if relevant)
float to_servo;
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  myServo.attach(servoPin);
}

void loop() {
  Wire.beginTransmission(MPU_addr);
  //wire address
  Wire.write(0x3B);
  Wire.endTransmission(false);
  //request from MPU address 14 bytes
  Wire.requestFrom(MPU_addr,14,true);

  //accelerometer communicates positional data
  wr_X=Wire.read()<<8|Wire.read();
  wr_Y=Wire.read()<<8|Wire.read();
  wr_Z=Wire.read()<<8|Wire.read();

  //maps wire read values to QI and QIV
  int xAng = map(wr_X,minVal,maxVal,-90,90);
  int yAng = map(wr_Y,minVal,maxVal,-90,90);
  int zAng = map(wr_Z,minVal,maxVal,-90,90); 

  //takes values and converts to QI, QII, QIII, and QIV values
  x = RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y = RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

  if (forY == true){
    if (y >= 0 && y <= 90){
      difference = y;
      angle_offset = 90 - difference;
    }
    else if(y>=270 && y<360){
      difference = y-270;
      angle_offset = 180-difference;
    }
    else if(y>=180 && y<270){
      if(origin<0){
        difference = y-180;
        angle_offset = -90+ difference;
        origin = -1*origin; 
      }
      else{
        angle_offset = 180;
      }
    }
    
  }
  else if(forX == true){
    if (x >= 0 && x <= 90){
      difference = x;
      angle_offset = 90.00 - difference;
      
    }
   else if(x>=270 && x<360){
      difference = x-270;
      angle_offset = 180-difference;
      
    }
    else if(x>=180 && x<270){
      if(origin<0){
        difference = x-180;
        angle_offset = -90+ difference;
        origin = -1*origin; 
      }
      else{
        angle_offset = 180;
      }
    }
   }
  
  to_servo = angle_offset + origin; 
  myServo.write(to_servo);
  
  Serial.print("Positional Information: ");
  Serial.print("(");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.print(z);
  Serial.print(")");
  Serial.println("  ");
  //Serial.println(angle_offset);
  //Serial.println(to_servo);
  
  delay(400);
  
  
}
