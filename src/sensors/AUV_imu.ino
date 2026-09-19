#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <Madgwick.h>


const float sampleRate=100.0f;
const float beta=0.04f;
Madgwick filter(sampleRate,beta);


MPU9250_asukiaaa mpu;
float ax,ay,az,gx,gy,gz;

float gx_bias=1.01;
float gy_bias=-1.83;
float gz_bias=-0.38;

float ax_off=-0.00;
float ay_off=-0.02;
float az_off=-0.11;



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
  mpu.beginGyro();
  //mpu.beginMag();
  //filter.begin(sampleRate);

  Serial.println("MPU9250 ready");
}

void loop() {
  mpu.accelUpdate();
  mpu.gyroUpdate();
 // mpu.magUpdate();

  Serial.print("ACC (g): ");
  //Serial.print(mpu.accelX()-ax_off);
  ax=mpu.accelX()-ax_off;
 // Serial.print(", ");
  //Serial.print(mpu.accelY()-ay_off);
  ay=mpu.accelY()-ay_off;
  //Serial.print(", ");
  //Serial.print(mpu.accelZ()-az_off);
  az=mpu.accelZ()-az_off;

  float norm=sqrt(ax*ax+ay*ay+az*az);
  ax=ax/norm;
  ay=ay/norm;
  az=az/norm;

  /*Serial.print("ax: ");
  Serial.println(ax);
  Serial.print("ay: ");
  Serial.println(ay);
  Serial.print("az: ");
  Serial.println(az);*/


  //Serial.print(" | GYRO (deg/s): ");
  //Serial.print(mpu.gyroX()-gx_bias);
  gx=(mpu.gyroX()-gx_bias)*DEG_TO_RAD;
  //Serial.print(", ");
  //Serial.print(mpu.gyroY()-gy_bias);
  gy=(mpu.gyroY()-gy_bias)*DEG_TO_RAD;
 // Serial.print(", ");
 // Serial.print(mpu.gyroZ()-gz_bias);
  gz=(mpu.gyroZ()-gz_bias)*DEG_TO_RAD;
  //Serial.println();
  /*Serial.print("gx: ");
  Serial.println(gx);
  Serial.print("gy: ");
  Serial.println(gy);
  Serial.print("gz: ");
  Serial.println(gz);
  Serial.println();*/

  filter.updateIMU(gx,gy,gz,ax,ay,az);

  float roll=filter.getRoll();
  float pitch=filter.getPitch();
  float yaw=filter.getYaw();

  Serial.print("Roll: ");
  Serial.println(roll,2);
  Serial.print("Pitch: ");
  Serial.println(pitch,2);
  




  /*Serial.print(" | MAG (uT): ");
  Serial.print(mpu.magX());
  Serial.print(", ");
  Serial.print(mpu.magY());
  Serial.print(", ");
  Serial.println(mpu.magZ());*/

  delay(10);
}
