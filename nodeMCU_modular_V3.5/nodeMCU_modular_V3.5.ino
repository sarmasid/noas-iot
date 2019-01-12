/* -------------------------------------------------------------
This code is written as a part of IIT Mandi IoT testbed project. 
I/We thank all the authors/organizations for providing libraries for different modules. 
*****************************************************************************
* last update: June 29, 2018 by Siddhartha Sarma (sarma.siddhartha@yahoo.com)
*/

#include<ESP8266WiFi.h>

#define SLEEPTIME 300 // 6 minutes
#define PINHIGHDURATION 5000
#define SETTLETIME 10000
unsigned long delayTime;

/*----Enable/disable the required modules here-----------*/
#define wifi_present 1
#define dht_present 1
#define ds18b20_present 1
#define multiAnalog 0
#define voltmeasure 1

/*------Macros----------------------------*/
#define NUMELEMENTS(x) (sizeof(x) /  sizeof(x[0]))

/*------header files and pins corresponding to modules are loaded here----------*/
#if voltmeasure
  #define voltCktPowerPin D8
  #define analogPin A0
  #define NUM_SAMPLES 10 //voltage value sampling
  char voltCharData[10];
  float voltData;
#endif

#if dht_present
  #include "dht_nodeMCU.h"
  #define DHTPIN D2
  char DHTtempData[20]; 
  char DHThumData[20];
  bool dhtStatus;
#endif

#if ds18b20_present
  #include "ds18b20_nodeMCU.h"
  #define ONE_WIRE_BUS D1
  char DStempData[20];
  bool ds18b20Status;
#endif

#if wifi_present 
  #include<EEPROM.h>
  #include "wifi_nodeMCU.h"
  bool wifiStatus;
  int addr =0;
  byte value_at_addr;
  int wifi_ssid_idx_eeprom;
  //IPAddress broker(AAA,BBB,CCC,DDD);
  const char* mqtt_server = "AAA.BBB.CCC.DDD";
  char *ssidList[] = {"XXXXXXX","XXXXXXXX", "XXXXXXXX","XXXXXXXXX","XXXXXXXX"};
  char *passList[]={"******","*******","*******","*********","*******"};
  unsigned int numSSID = NUMELEMENTS(ssidList);
  WiFiClient esp;
  PubSubClient client(mqtt_server,1883,esp);
  //PubSubClient client(esp);
  int mqttStatus;
#endif

#if multiAnalog
  #define analogPin A0
  char soilMoistData[30];
  int pinArr[] = {D5,D6,D7,D8};
#endif


void setup() {
    Serial.begin(9600);
    
    Serial.println("Entered setup");
#if wifi_present    
    EEPROM.begin(512);
    value_at_addr = EEPROM.read(addr);
    wifi_ssid_idx_eeprom = (int) value_at_addr;
    Serial.println( wifi_ssid_idx_eeprom);
    
    if (wifi_ssid_idx_eeprom < numSSID){
       wifiStatus=wifi_init(ssidList[wifi_ssid_idx_eeprom], passList[wifi_ssid_idx_eeprom]);
    }

    if(wifiStatus){
        Serial.println("WiFi connected");
        client.setCallback(callback);
    }else{
       for(int i=0; i<numSSID; i++){
            Serial.print("Attempting to connect to: ");
            Serial.println(ssidList[i]);
            wifiStatus=wifi_init(ssidList[i], passList[i]);
         if(wifiStatus)
          {
            client.setCallback(callback);
            //client.setServer(broker,1883);
            Serial.println("WiFi connected");
            EEPROM.write(addr,i);
            EEPROM.commit();
            Serial.println("Stored the ssid index in EEPROM");
            break;
          }else if(i< numSSID-1){
               Serial.println("");
               Serial.println("Reached maximum attempt count. Trying the next SSID from list");
           }else{
            Serial.println("Reached end of SSID list");
           }
              
         }
      }
#endif  
}



void loop() { 
#if multiAnalog
   readMultiAnalog(pinArr, soilMoistData);
   Serial.println(soilMoistData);
#endif

#if voltmeasure
  voltData = voltMeasure();
  Serial.println(voltData);
  dtostrf(voltData,4,2,voltCharData);
  Serial.println(voltCharData);
#endif

#if dht_present
  dhtStatus = dht_read(DHTtempData, DHThumData, DHTPIN);
  if(dhtStatus){
  Serial.print("DHT temperature: ");
  Serial.println(DHTtempData);
  Serial.print("DHT humidity: ");
  Serial.println(DHThumData);
  }else{
    Serial.println("Failed to read from dht");
  }
#endif

#if ds18b20_present
  ds18b20Status = ds18b20_read(DStempData, ONE_WIRE_BUS);
  if(ds18b20Status){
  Serial.print("ds18b20 reading: ");
  Serial.println(DStempData);
  }else{
     Serial.println("Failed to read from ds18b20");
  }
#endif

#if wifi_present
 if(wifiStatus){
  if (!client.connected()) {
       mqttStatus = reconnectMCU(client, node_id);
     }else{
       mqttStatus = 0;
     }
     
 if(mqttStatus==0){
       readCurrTime(client);
       Serial.println(currentTime);
       publishTopic(client, "status","check","ok");
       
       #if dht_present
         if(dhtStatus){
           publishTopic(client, "dht22", "status","ok");
           publishTopic(client, "dht22","temp",DHTtempData);
           publishTopic(client, "dht22","hum",DHThumData);
         }else{
           publishTopic(client, "dht22", "status","read failed");
         }
        #endif
        
        #if ds18b20_present
          if(ds18b20Status){
            publishTopic(client, "ds18b20", "status","ok");
            publishTopic(client, "ds18b20","temp",DStempData);
          }else{
             publishTopic(client, "ds18b20", "status","read failed");
          }
        #endif

        #if multiAnalog
            publishTopic(client, "soilMoist","all",soilMoistData);
        #endif

        #if voltmeasure
            publishTopic(client, "Battery", "Volt", voltCharData);
        #endif
        client.disconnect();
        delay(1000);
   }else{
          Serial.print("MQTT connection failed. client state: ");
          Serial.println(mqttStatus);
        }
     
       WiFi.disconnect(); //uncomment it if we want to implement sleepmode
      delay(1000);
     }
  else{
  Serial.println("Unable to connect to WiFi, so failed to post data");
 }
 #endif
 
  Serial.println(F("Going to sleep"));
  ESP.deepSleep(SLEEPTIME*1000000,WAKE_RF_DEFAULT); // implement the sleep mode
  
} 

#if multiAnalog
 void readMultiAnalog(int *pinArrPtr, char *charMoistData)
 {
   String moistData;
   for(int i=0; i<NUMELEMENTS(pinArr); i++){
   Serial.print("Reading from soil moisture ");
   Serial.println(i);
    pinMode(pinArr[i], OUTPUT);
    digitalWrite(pinArr[i],HIGH);
    delay(PINHIGHDURATION);
    moistData=String(moistData + ","+  String(map(funMoistRead(),950,0,0,100))); //String(random(40,70))); //
    digitalWrite(pinArr[i],LOW);
    delay(SETTLETIME);
   }
   moistData.toCharArray(charMoistData,30);
   Serial.println(charMoistData);
 }

 float funMoistRead(){
   int moistReadVal =0;
    for(int i=0;i<10; i++){
      moistReadVal= moistReadVal+ analogRead(analogPin);
     //Serial.println(moistReadVal);
     delay(100); 
   }
   
   return moistReadVal/10; 
}
#endif

#if voltmeasure
float voltMeasure()
     {
        unsigned int sample_count=0;
        float voltage;
        float sum =0;
        pinMode(voltCktPowerPin,OUTPUT);
        digitalWrite(voltCktPowerPin, HIGH);       // sets the digital pin 13 on
        while (sample_count < NUM_SAMPLES) // take a number of analog samples and add them up
        {
            sum += analogRead(analogPin);
            sample_count++;
            delay(10);
        }
      voltage = ((float)sum / (float)NUM_SAMPLES *12.15) / 1024.0; // voltage calculation 
      Serial.print(voltage); // send voltage for display on Serial Monitor
      Serial.println (" V"); // serial print V
      sample_count = 0;
      sum = 0; // Reset count
      digitalWrite(voltCktPowerPin, LOW);      
      return voltage;
     }
#endif
