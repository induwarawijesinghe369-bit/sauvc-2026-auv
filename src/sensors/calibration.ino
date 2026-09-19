#include <Wire.h>
#include <QMC5883LCompass.h>


QMC5883LCompass mag;

float x_off=2428;
float y_off=652.5;
float z_off=-4255;

float x_scale=1.036;
float y_scale=0.98;
float z_scale=0.99;





void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Init magnetometer
  mag.init();
  

  Serial.println("GY-271 Magnetometer Ready");


}

void loop() {

  // ================== READ MAG ==================
  mag.read();

  float mx = mag.getX();
  float my = mag.getY();
  float mz = mag.getZ();

  mx=mx-x_off;
  my=my-y_off;
  mz=mz-z_off;

  mx=mx*x_scale;
  my=my*y_scale;
  mz=mz*z_scale;

  float c=mx*mx+my*my+mz*mz;
  float norm=sqrt(c);

  mx=mx/norm;
  my=my/norm;
  mz=mz/norm;


  


 

  delay(500); // 100 Hz
}
