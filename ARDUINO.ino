
#include "Wire.h"
#include <MPU6050_light.h>
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
int dht = 4;
int count=0;

MPU6050 mpu(Wire);


void setup () {
  Serial.begin(9600);
 pinMode(dht, INPUT);
  Wire.begin();
 
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
 
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
}

void loop () {

   mpu.update();

DHT.read(dht);  
int dhtvalue = DHT.temperature;
Serial.print("X");
Serial.print(dhtvalue);
Serial.println("#");


   Serial.print("accerelometer : ");
  Serial.print(abs(mpu.getAngleX()));
  Serial.println("#");


//Serial.print("X : ");
//Serial.print(dhtvalue);
//Serial.println("#");


   Serial.print("Y");
  Serial.print(abs(mpu.getAngleX()));
  Serial.println("#");


  if (abs(mpu.getAngleX()) > 50)
  {
  Serial.println("FALL");

  }

  
}
