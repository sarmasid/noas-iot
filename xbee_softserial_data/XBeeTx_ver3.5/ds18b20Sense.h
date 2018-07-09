#include<OneWire.h>
#include<DallasTemperature.h>
#define LOOPCOUNTMAX 5

bool ds18b20_read(float *DStempData, unsigned int ONE_WIRE_BUS)
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
  }
  *DStempData = localTempVar;
    if(localTempVar==85 || localTempVar == -127)
    return 0;
  else
    return 1;
}
