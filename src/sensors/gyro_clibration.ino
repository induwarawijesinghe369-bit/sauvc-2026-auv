#include <Wire.h>
#include <MPU9250_asukiaaa.h>

MPU9250_asukiaaa mpu;
double gx_sum=0;
double gy_sum=0;
double gz_sum=0;
int count=0;


void setup() {
  Serial.begin(115200);
  

#if defined(ESP32)
  Wire.begin(21, 22);   // ESP32 I2C pins
#else
  Wire.begin();         // Arduino UNO
#endif

  Serial.println("Initializing MPU9250...");

  mpu.setWire(&Wire);
  //mpu.beginAccel();
  mpu.beginGyro();
  //mpu.beginMag();

  Serial.println("MPU9250 ready");
}

void loop(){
  mpu.gyroUpdate();
  gx_sum=gx_sum+mpu.gyroX();
  gy_sum=gy_sum+mpu.gyroY();
  gz_sum=gz_sum+mpu.gyroZ();
  count=count+1;
  Serial.print("count: ");
  Serial.println(count);
  Serial.print("sum_x: ");
  Serial.println(gx_sum);
  Serial.print("sum_x: ");
  Serial.println(gy_sum);
  Serial.print("sum_x: ");
  Serial.println(gz_sum);



  delay(500);


}
