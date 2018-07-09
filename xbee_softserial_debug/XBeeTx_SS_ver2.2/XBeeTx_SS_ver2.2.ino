/* -------------------------------------------------------------
This code is written as a part of IIT Mandi IoT testbed project. 
I/We thank all the authors/organizations for providing libraries for different modules. 
*****************************************************************************
* last update: July 7, 2018 by Siddhartha Sarma (sarma.siddhartha@yahoo.com)
*/

#include <SoftwareSerial.h>
#include<LowPower.h>
#include "dhtSense.h"
//#include<SoftwareReset.h>


#define LOW_POWER_PERIOD 8
uint8_t ssRX = 8;
uint8_t ssTX = 9;
SoftwareSerial swSerial(ssRX, ssTX);
#include "Xbeemodule.h"
char RX_ADDRESS[] = "0x416C44C1";
int error;
long fNo = 0; 
int rssiReading;
dbCmd_Rsp_t rssiValTx;
XBee_Packet_t xbeepkt;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
xbee.setSerial(Serial);
swSerial.begin(9600);
//xbee.begin(Serial);
swSerial.println("Tx starting....");
}

void loop() {
  sendAtCommand(dbCmd, &rssiValTx); 
  swSerial.println(-1*rssiValTx.rssi);
 /*--------populating the data---------------*/
  fNo++;
  xbeepkt.sensData.frameNo = fNo;

  /* ------------ds18b20 data------------------*/
  xbeepkt.sensData.ds18b20Data.ds18b20code = 'a';
  xbeepkt.sensData.ds18b20Data.ds18b20TempVal = randFloat(); 

  /*-------------bme280-------------------------*/
  xbeepkt.sensData.bmeData.bme280code = 'b';
  xbeepkt.sensData.bmeData.bme280TempVal = randFloat();
  xbeepkt.sensData.bmeData.bme280HumVal = randFloat();
  xbeepkt.sensData.bmeData.bme280PresVal = randFloat();
  
/*------------------dht --------------------------*/
  xbeepkt.sensData.dhtData.dhtcode = 'c';
  xbeepkt.sensData.dhtData.dhtTempVal = randFloat();
  xbeepkt.sensData.dhtData.dhtHumVal = randFloat();

/*------------------battery-------------------------*/
  xbeepkt.sensData.batteryData.batterycode = 'd';
  xbeepkt.sensData.batteryData.batteryVoltVal = randFloat();

  /*----------------rssi-------------------------*/
  xbeepkt.sensData.rssiData.rssicode = 'e';
  xbeepkt.sensData.rssiData.rssiVal = -1*rssiValTx.rssi;

  
  error = Xbsend(xbeepkt); //RX_ADDRESS, "randata", val);

  if(error==0){
    swSerial.println("Data sent successfully!");
  }else{
      swSerial.print("Error occured (code):");
      swSerial.println(error);
    }
  delay(10000);
  //swSerial.end();
  arduinoSleep(1);
  //swSerial.begin(9600);
 //if(fNo==255) softwareReset::standard();
}

void arduinoSleep(long idlePeriodInMin){
  unsigned int nCycle;
  nCycle = idlePeriodInMin*60/LOW_POWER_PERIOD;
   for (int i=0; i<nCycle; i++) {  
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    //swSerial.print(".");
    //swSerial.flush();
    delay(10); 
   }
  swSerial.println();
}

float randFloat(){
  return random(0,9999)/100.0;
}

float *multiRandFloat(){
  float multVals[2];
  multVals[0] = randFloat();
  multVals[1] = randFloat();
  return multVals;
}

