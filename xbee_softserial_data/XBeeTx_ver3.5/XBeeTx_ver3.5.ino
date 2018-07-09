/* -------------------------------------------------------------
This code is written as a part of IIT Mandi IoT testbed project. 
I/We thank all the authors/organizations for providing libraries for different modules. 
*****************************************************************************
* last update: July 7, 2018 by Siddhartha Sarma (sarma.siddhartha@yahoo.com)
*/
#include <SoftwareSerial.h>
#include<LowPower.h>
#include "ds18b20Sense.h"
#include "dhtSense.h"
//#include<SoftwareReset.h>

#define DHTPOWERPIN 5 // pin D2
#define DHTPIN 6 // pin D3
#define DS18B20POWERPIN 7 // pin D4
#define DS18B20DATAPIN 10 // pin D5
#define LM35POWERPIN 11 //pin D6
#define LM35READPIN A0
#define VOLTMEASUREPOWERPIN 12 // pin D7
#define VOLTREADPIN A1
#define NUM_SAMPLES 10 //voltage value sampling

#define LOW_POWER_PERIOD 8
uint8_t ssRX = 8;
uint8_t ssTX = 9;
SoftwareSerial swSerial(ssRX, ssTX);
#include "Xbeemodule.h"
int error;
long fNo = 0; 
boolean ds18b20_status;
float ds18b20Reading; 
boolean dht_status;
float dhtTempReading;
float dhtHumReading;
float lm35Reading;
float batteryVoltReading;
int rssiReading;
dbCmd_Rsp_t rssiValTx;
XBee_Packet_t xbeepkt;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
swSerial.begin(9600);
xbee.begin(swSerial);

pinMode(DHTPOWERPIN, OUTPUT);
pinMode(DS18B20POWERPIN, OUTPUT);
pinMode(LM35POWERPIN, OUTPUT);
pinMode(VOLTMEASUREPOWERPIN,OUTPUT);

Serial.println(F("Tx starting...."));
}

void loop() {
  /*-------read all the sensor data---------------*/
  sendAtCommand(dbCmd, &rssiValTx); 
  //Serial.println(-1*rssiValTx.rssi);
  
  digitalWrite(DS18B20POWERPIN,HIGH);
  delay(5000);
  ds18b20_status = ds18b20_read(&ds18b20Reading,DS18B20DATAPIN);
  digitalWrite(DS18B20POWERPIN,LOW);
  delay(1000);
  
 // digitalWrite(DHTPOWERPIN,HIGH);
  delay(5000);
  dht_status = 0; //dht_read(&dhtTempReading, &dhtHumReading, DHTPIN);
 // digitalWrite(DHTPOWERPIN,LOW);
//  Serial.println(dhtTempReading);
//  Serial.println(dhtHumReading);
  
  lm35Reading = lm35TempMeasure();
  batteryVoltReading = voltMeasure();
  //Serial.println(batteryVoltReading);
 /*--------populating the data---------------*/
  fNo++;
  xbeepkt.sensData.frameNo = fNo;

  /* ------------ds18b20 data------------------*/
  xbeepkt.sensData.ds18b20Data.ds18b20code = 'a';
 
  if(ds18b20_status){
   xbeepkt.sensData.ds18b20Data.ds18b20TempVal = ds18b20Reading; 
  }else{
   xbeepkt.sensData.ds18b20Data.ds18b20TempVal =0.0; 
  }
  /*-------------bme280-------------------------*/
  xbeepkt.sensData.bmeData.bme280code = 'b';
  xbeepkt.sensData.bmeData.bme280TempVal = randFloat();
  xbeepkt.sensData.bmeData.bme280HumVal = randFloat();
  xbeepkt.sensData.bmeData.bme280PresVal = randFloat();
  
/*------------------dht --------------------------*/
  xbeepkt.sensData.dhtData.dhtcode = 'c';
  if(dht_status){
  xbeepkt.sensData.dhtData.dhtTempVal = dhtTempReading; 
  xbeepkt.sensData.dhtData.dhtHumVal = dhtHumReading;
  }else{
  xbeepkt.sensData.dhtData.dhtTempVal = 0.0; 
  xbeepkt.sensData.dhtData.dhtHumVal = 0.0;
  }
/*-------------------lm35---------------------------*/
 xbeepkt.sensData.lm35Data.lm35code = 'd';
 xbeepkt.sensData.lm35Data.lm35TempVal = lm35Reading;
/*------------------battery-------------------------*/
  xbeepkt.sensData.batteryData.batterycode = 'e';
  xbeepkt.sensData.batteryData.batteryVoltVal = batteryVoltReading;
  //Serial.println(xbeepkt.sensData.batteryData.batteryVoltVal);
 /*----------------rssi-------------------------*/
  xbeepkt.sensData.rssiData.rssicode = 'e';
  xbeepkt.sensData.rssiData.rssiVal = -1*rssiValTx.rssi;
  
  error = Xbsend(xbeepkt); //RX_ADDRESS, "randata", val);

  if(error==0){
    Serial.println("Data sent successfully!");
  }else{
      Serial.print("Error occured (code):");
      Serial.println(error);
    }
  delay(10000);
 arduinoSleep(1);

 //if(fNo==255) softwareReset::standard();
}

void arduinoSleep(long idlePeriodInMin){
  unsigned int nCycle;
  nCycle = idlePeriodInMin*60/LOW_POWER_PERIOD;
   for (int i=0; i<nCycle; i++) {  
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
//    Serial.print(".");
//    Serial.flush();
    delay(10); 
   }
  Serial.println();
}

float randFloat(){
  return random(0,9999)/100.0;
}

float voltMeasure(){  
     // analogReference(DEFAULT);
      unsigned int sample_count=0;
      float voltage;
      float sum =0;
      digitalWrite(VOLTMEASUREPOWERPIN, HIGH);       // sets the digital pin 13 on
      while (sample_count < NUM_SAMPLES) // take a number of analog samples and add them up
      {
          sum += analogRead(VOLTREADPIN);
          sample_count++;
          delay(10);
      }
    voltage = ((float)sum / (float)NUM_SAMPLES *17.55) / 1023.0; // voltage calculation 
   // Serial.print(voltage); // send voltage for display on Serial Monitor
   // Serial.println (" V"); // serial print V
    sample_count = 0;
    sum = 0; // Reset count
    digitalWrite(VOLTMEASUREPOWERPIN, LOW);      
    return voltage;
     }

float lm35TempMeasure(){
  //analogReference(INTERNAL); //for improving accuracy of analog reading https://playground.arduino.cc/Main/LM35HigherResolution
  int tempReading = 0;
  digitalWrite(LM35POWERPIN, HIGH);
  delay(1000);
  for(int i=0; i < NUM_SAMPLES; i++){
    tempReading = tempReading + analogRead(LM35READPIN);
  }
  //Serial.println(tempReading);
  digitalWrite(LM35POWERPIN, LOW);
  return tempReading*5.0*100.0/(1024*NUM_SAMPLES);
}
     

