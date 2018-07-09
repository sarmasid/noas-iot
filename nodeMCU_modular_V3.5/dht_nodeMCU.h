#include <DHT.h> 
#include <Adafruit_Sensor.h>

bool dht_read(char *DHTtemp, char *DHThum, unsigned int DHTPIN)
{
 float localVarTemp, localVarHum;
 uint8_t loopCount = 0;
 DHT dht(DHTPIN, DHT11);
 dht.begin();
 do{
 delay(2000);
 localVarTemp = dht.readTemperature();
 localVarHum = dht.readHumidity();
// Serial.println(localVarTemp);
// Serial.println(localVarHum);
 //Serial.println(loopCount);
 loopCount++;
 }while((isnan(localVarTemp) || isnan(localVarHum)) && loopCount <5);
 
 //String tempStr=String(localVarTemp);
 //Serial.println(localVarTemp);
 
 //Serial.println(tempStr);
 //tempStr.toCharArray(DHTtemp,20);
 //Serial.println(DHTtemp);
// String hum=String(localVarHum);// dht.readHumidity());
// hum.toCharArray(DHThum,20);
// Serial.println(DHThum);

 dtostrf(localVarTemp,4,2,DHTtemp);
 dtostrf(localVarHum,4,2,DHThum);
 if (isnan(localVarTemp) || isnan(localVarHum))
  return 0;
 else
  return 1;
}
