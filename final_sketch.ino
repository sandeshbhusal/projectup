#include <SD.h>
#include <SPI.h>
#include <string.h>
#include "TinyGPS++.h"
#include "SoftwareSerial.h"
//#include "MPU6050.h"
#include <Ethernet.h>
//#include "I2Cdev.h"
//#include "Wire.h" 

//MPU6050 accelgyro;
TinyGPSPlus gps;
SoftwareSerial mySerial(9, 8);
//File file;


char server[] = "pulchowk.000webhostapp.com";
EthernetClient client;

//#define IMUInt      2
#define SDPin       4
#define BAUD        9600

void setup(){
    Serial.begin(BAUD);
    mySerial.begin(BAUD);
    while(!Serial);

    Serial.println(F("Initiating SD Card..."));
    if (!SD.begin(4)) {
        Serial.println(F("SD Card initialization failed!"));
        return;
    }
    
    delay(1000);
    Serial.println(F("Initiating Ethernet Connection..."));
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    IPAddress ip(192, 168, 0, 177);

    if (Ethernet.begin(mac) == 0) {
        Serial.println(F("Failed to configure Ethernet using DHCP"));
        Ethernet.begin(mac, ip);
    }
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
        
            if(gps.location.isUpdated()){
              Serial.println(F("The thing workings!"));
                String Data =   String(gps.location.lat(), 9);
                Data +=   String(gps.location.lng(), 9)+"|";
                Data +=   String(gps.altitude.value())+"|";
                Data +=   String(gps.satellites.value())+"|";
                Data +=   String(gps.date.value())+"|";
                Data +=   String(gps.time.value())+"|";
                Serial.println((Data));

                      File file = SD.open("data.txt", FILE_WRITE);
                      if (file) {
                        Serial.print(F("Writing to data.txt..."));
                        file.println(Data);
                        file.close();
                        Serial.println(F("done."));
                      } else {
                        Serial.println(F("error opening data.txt"));
                      }

                      post(Data);
                      Data = "";
            }
}

static void post(String input){
    delay(1000);
    Serial.println(F("connecting..."));

    String data = "data="+input;
    if (client.connect(server, 80)) {
      Serial.println(F("Connected!"));
      client.println(F("POST / HTTP/1.1")); 
      client.print(F("Host: pulchowk.000webhostapp.com")); // SERVER ADDRESS HERE TOO
      client.println(F("Content-Type: application/x-www-form-urlencoded")); 
      client.print(F("Content-Length: ")); 
      client.println(data.length()); 
      client.println(); 
      client.println(data);
      client.println(F("Connection: close"));
      Serial.println(F("Sent data! to HTTP Server!"));
    } else {
      Serial.println(F("connection failed"));
    }
    delay(5000);
    client.stop();
    delay(5000);
}
