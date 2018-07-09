#include <DHT.h> 
#include <Adafruit_Sensor.h>

bool dht_read(float *DHTtemp, float *DHThum, unsigned int DHTPIN)
{
 float localVarTemp, localVarHum;
 uint8_t loopCount = 0;
 DHT dht(DHTPIN, DHT11);
 dht.begin();
 do{
     delay(2000);
     localVarTemp = dht.readTemperature();
     localVarHum = dht.readHumidity();
     loopCount++;
 }while((isnan(localVarTemp) || isnan(localVarHum)) && loopCount <5);

 *DHTtemp = localVarTemp;
 *DHThum = localVarHum; 
 
 if (isnan(localVarTemp) || isnan(localVarHum))
  return 0;
 else
  return 1;
}
