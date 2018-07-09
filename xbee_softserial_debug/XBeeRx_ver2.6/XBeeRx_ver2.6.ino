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
dbCmd_Rsp_t rssiValRx;
XBee_Packet_t rdxbpkt; 
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
    Serial.print(F("Frame # : "));
    Serial.print(rdxbpkt.sensData.frameNo);
    Serial.println(rdxbpkt.sensData.bmeData.bme280TempVal);
    Serial.print(F("RSSI at Tx: "));
    Serial.print(rdxbpkt.sensData.rssiData.rssiVal);
    Serial.println(F(" dBm"));
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
  delay(5000);
}
