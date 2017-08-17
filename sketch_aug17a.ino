#include <SD.h>
#include <SPI.h>
#include <string.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <MPU6050.h>
#include <I2Cdev.h>

MPU6050 accelgyro;
TinyGPSPlus gps;
SoftwareSerial mySerial(9, 8);

#define IMUInt      2
#define SDPin       4
#define BAUD        9600

void setup(){
    Serial.begin(BAUD);
    mySerial.begin(BAUD);
    while(!Serial);
    Serial.println(F("Initializing MPU device..."));

    accelgyro.initialize();
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    accelgyro.setXAccelOffset(501);
    accelgyro.setYAccelOffset(619);
    accelgyro.setZAccelOffset(2061);
    accelgyro.setXGyroOffset(-26);
    accelgyro.setYGyroOffset(-12);
    accelgyro.setZGyroOffset(35);
    
    Serial.println(F("Initiating SD Card..."));
    if (!SD.begin(4)) {
      Serial.println(F("SD Card initialization failed!"));
      return;
    }
    
    delay(1000);
    
    Serial.println(F("Configuration Complete!"));
    delay(1000);
}


int milli = 0;

void loop() {
  while(mySerial.available()>0){
    char data = mySerial.read();
    gps.encode(data);
    Serial.write(data);
  }
  int16_t ax, ay, az, gx, gy, gz;
  double time;
    
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  String out = "";
  out += String(ax);
  out += String(ay); 
  out += String(az); 
  out += String(gx); 
  out += String(gy); 
  out += String(gz); 
  time = millis();
  out += String(time);

  File file = SD.open("data.txt", FILE_WRITE);
  if (file) {
    Serial.print(F("Writing to data.txt..."));
    file.println(out);
    if(GPSLogger() == "")
      file.close();
    else{
        file.println(GPSLogger());
        file.close();
      }
    Serial.println(F("done."));
  }
  else {
    Serial.println(F("error opening data.txt"));
  }
}

String GPSLogger(){  
  if(gps.location.isUpdated()){
    Serial.println(F("The thing workings!"));
    String Data =   String(gps.location.lat(), 9);
    Data +=   String(gps.location.lng(), 9)+"|";
    Data +=   String(gps.altitude.value())+"|";
    Data +=   String(gps.satellites.value())+"|";
    Data +=   String(gps.date.value())+"|";
    Data +=   String(gps.time.value())+"|";
    Data += ",";
    return Data;
  }
  return "";
}
