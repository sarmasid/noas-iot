/* -------------------------------------------------------------
This code is written as a part of IIT Mandi IoT testbed project. 
I/We thank all the authors/organizations for providing libraries for different modules. 
*****************************************************************************
* last update: July 7, 2018 by Siddhartha Sarma (sarma.siddhartha@yahoo.com)
*/
#include <SoftwareSerial.h>
uint8_t ssRX = 8;
uint8_t ssTX = 9;
SoftwareSerial swSerial(ssRX, ssTX);
#include "Xbeemodule.h"

#define ds18b20_present 1
#define bme280_present 0
#define dht_present 0
#define lm35_present 1
#define battery_measure 1

XBee_Packet_t rdxbpkt; 
dbCmd_Rsp_t rssiValRx;
uint16_t packetLength = 0;
uint16_t payloadLength = 0;
int error;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
swSerial.begin(9600);
xbee.begin(swSerial);
Serial.println(F("Starting up!"));
}

void loop() {
  // checking if some data is available in swSerial
 if(swSerial.available()){
   error= Xbrecv(10000, &rdxbpkt, &payloadLength, &packetLength);
   if(error==0){
     Serial.print(F("packet length: "));
    Serial.println(packetLength,DEC);
    Serial.print(F("payload length: "));
    Serial.println(payloadLength,DEC);
    Serial.print(F("Frame# "));
    Serial.println(rdxbpkt.sensData.frameNo);
  #if ds18b20_present
    Serial.print(F("DS18B20 reading "));
    Serial.println(rdxbpkt.sensData.ds18b20Data.ds18b20TempVal);
  #endif 
  
  #if bme280_present
    Serial.print(F("BME280 temp: "));
    Serial.println(rdxbpkt.sensData.bmeData.bme280TempVal);
    Serial.print(F("BME280 Hum: "));
    Serial.println(rdxbpkt.sensData.bmeData.bme280HumVal);
    Serial.print(F("BME280 Pres: "));
    Serial.println(rdxbpkt.sensData.bmeData.bme280PresVal);
  #endif

  #if dht_present
    Serial.print(F("DHT temp: "));
    Serial.println(rdxbpkt.sensData.dhtData.dhtTempVal);
    Serial.print(F("DHT Hum: "));
    Serial.println(rdxbpkt.sensData.dhtData.dhtHumVal);
  #endif 

  #if lm35_present
    Serial.print(F("LM35 Temp reading: "));
    Serial.println(rdxbpkt.sensData.lm35Data.lm35TempVal);
  #endif
  
  #if battery_measure 
    Serial.print(F("Battery voltage: "));
    //Serial.print(rdxbpkt.sensData.batteryData.batterycode);
    Serial.println(rdxbpkt.sensData.batteryData.batteryVoltVal);
  #endif

    Serial.print(F("RSSI of Tx: "));
    Serial.println(rdxbpkt.sensData.rssiData.rssiVal);
  delay(1000);
  }else{
      Serial.print("Error occured (code):");
      Serial.println(error);
    }

   Serial.print(F("RSSI: "));
   sendAtCommand(dbCmd, &rssiValRx); 
   Serial.print(-1*rssiValRx.rssi);
   Serial.println(F(" dBm"));
   Serial.println(F("--------------------"));
   swSerial.flush();
 }
   delay(1000);
}


