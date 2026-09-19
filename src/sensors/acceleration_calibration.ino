#include <Wire.h>
#include <MPU9250_asukiaaa.h>

MPU9250_asukiaaa mpu;
double gx_bias=1.01;
double gy_bias=-1.83;
double gz_bias=-0.38;

double ax_sum=0;
double ay_sum=0;
double az_sum=0;

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
  mpu.beginAccel();
  //mpu.beginGyro();
  //mpu.beginMag();

  Serial.println("MPU9250 ready");
}

void loop() {
  mpu.accelUpdate();
  //mpu.gyroUpdate();
 // mpu.magUpdate();
 ax_sum=ax_sum+mpu.accelX();
 ay_sum=ay_sum+mpu.accelY();
 az_sum=az_sum+mpu.accelZ()+1;

 count=count+1;

 Serial.print("count: ");
 Serial.println(count);

 Serial.print("ax: ");
 Serial.println(ax_sum);
 Serial.print("ay: ");
 Serial.println(ay_sum);
 Serial.print("az: ");
 Serial.println(az_sum);



  /*Serial.print("ACC (g): ");
  Serial.print(mpu.accelX());
  Serial.print(", ");
  Serial.print(mpu.accelY());
  Serial.print(", ");
  Serial.print(mpu.accelZ());*/

  /*Serial.print(" | GYRO (deg/s): ");
  Serial.print(mpu.gyroX()-gx_bias);
  Serial.print(", ");
  Serial.print(mpu.gyroY()-gy_bias);
  Serial.print(", ");
  Serial.print(mpu.gyroZ()-gz_bias);
  Serial.println();*/

  /*Serial.print(" | MAG (uT): ");
  Serial.print(mpu.magX());
  Serial.print(", ");
  Serial.print(mpu.magY());
  Serial.print(", ");
  Serial.println(mpu.magZ());*/

  delay(500);
}
