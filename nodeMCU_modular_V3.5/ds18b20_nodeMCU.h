#include<OneWire.h>
#include<DallasTemperature.h>
#define LOOPCOUNTMAX 5

bool ds18b20_read(char *DStempData, unsigned int ONE_WIRE_BUS)
{
  float localTempVar;
  uint8_t loopCount = 0;
  OneWire oneWire(ONE_WIRE_BUS); 
  DallasTemperature sensors(&oneWire);
  sensors.requestTemperatures(); // for getting temperature from DS18B20
  localTempVar = sensors.getTempCByIndex(0);
  while(((int)localTempVar==85 || (int)localTempVar == -127) && loopCount < LOOPCOUNTMAX){
    localTempVar = sensors.getTempCByIndex(0);
    loopCount++;
    //Serial.println(loopCount);
    //Serial.println(localTempVar);
  }
  //String DStemp=String(sensors.getTempCByIndex(0));
//  String DStemp=String(localTempVar);
//  DStemp.toCharArray(DStempData,20);
 dtostrf(localTempVar,4,2,DStempData);
  if(localTempVar==85 || localTempVar == -127)
    return 0;
  else
    return 1;
}
