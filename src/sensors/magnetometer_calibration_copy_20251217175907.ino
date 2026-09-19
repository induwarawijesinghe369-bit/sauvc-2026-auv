#include <Wire.h>
#include <QMC5883LCompass.h>


QMC5883LCompass mag;

float magx_min,magy_min,magz_min;
float magx_max,magy_max,magz_max;





void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Init magnetometer
  mag.init();
  

  Serial.println("GY-271 Magnetometer Ready");

  mag.read();

  float mx = mag.getX();
  float my = mag.getY();
  float mz = mag.getZ();

  magx_min=mag.getX();
  magy_min=mag.getY();
  magz_min=mag.getZ();

  magx_max=mag.getX();
  magy_max=mag.getY();
  magz_max=mag.getZ();


}

void loop() {

  // ================== READ MAG ==================
  mag.read();

  float mx = mag.getX();
  float my = mag.getY();
  float mz = mag.getZ();

  if(magx_min>mx){

    magx_min=mx;
  }

  if(magy_min>my){

    magy_min=my;
  }

  if(magz_min>mz){

    magz_min=mz;
  }

  if(magx_max<mx){

    magx_max=mx;
  }

  if(magy_max<my){

    magy_max=my;
  }

  if(magz_max<mz){

    magz_max=mz;
  }



  Serial.print("magx_min: ");
  Serial.print(magx_min);
  Serial.print(",");
  Serial.print("magy_min: ");
  Serial.print(magy_min);
  Serial.print(",");
  Serial.print("magz_min: ");
  Serial.print(magz_min);
  Serial.print("         ");

  Serial.print("magx_max: ");
  Serial.print(magx_max);
  Serial.print(",");
  Serial.print("magy_max: ");
  Serial.print(magy_max);
  Serial.print(",");
  Serial.print("magz_max: ");
  Serial.println(magz_max);



 

  delay(500); // 100 Hz
}
