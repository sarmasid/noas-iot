#include<ESP8266WiFi.h>
#include<WiFiClient.h>
#include<PubSubClient.h>
#include<stdlib.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// assign the ESP8266 pins to arduino pins
#define D1 5
#define D2 4
#define D4 2
#define D3 0

// assign the SPI bus to pins
#define BME_SCK D1
#define BME_MISO D4
#define BME_MOSI D2
#define BME_CS D3

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

/*#define DHTPIN 4
#define DHTTYPE DHT22
#define MOIST 5
#define LED D0*/
#define MAXATTEMPT 5
#define SLEEPTIME 300 // 5 minutes
//DHT dht(DHTPIN,DHTTYPE);


/* ---Connection configuration--- */
IPAddress broker(<put_broker_ip_here>);
//IPAddress staticip(10,8,11,29);  // can uncomment this line to make the ip of the node MCU static
//IPAddress gateway(10,8,0,1);
//IPAddress subnet(255,255,0,0);
const char *ssid = "<put_ssid_here>";
const char *pass =  "<put_password_here>";
WiFiClient esp;
PubSubClient client(esp);
char allData[40]; 
char tempData[20];
char presData[20];
char humData[20];
char altData[20];
bool BMEstatus;

void setup(){
  Serial.begin(115200);
  //pinMode(LED, OUTPUT);
  Serial.println("Entered Set-up");
  //WiFi.config(staticip,gateway,subnet,gateway);  // uncomment this in case you want a static ip
  WiFi.begin(ssid,pass);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected to Access Point :");
  Serial.println(ssid);
  Serial.print("IP address :: ");
  Serial.println(WiFi.localIP());
  client.setServer(broker,1883);
  
}

void loop() {
// r["bn"]="NodeMCU_01";
    /* ---send data--- */
   if (!client.connected()) {
       reconnectMCU();
     }
  else
    {
    Serial.println("Failed to connect!");
    //client.stop();
    }
  //client.loop();
  client.disconnect();
  delay(1000);
  WiFi.disconnect(); //uncomment it if we want to implement sleepmode
  //digitalWrite(LED, HIGH);
  delay(1000);
  Serial.println("Going to sleep");
  ESP.deepSleep(SLEEPTIME*1000000,WAKE_RF_DEFAULT); // implement the sleep mode
  
}


void reconnectMCU() {
  
   int attempt = 1;
   while (!client.connected()) {
       Serial.print("Attempting MQTT connection...");
    
    if (client.connect("nodeMCU001")) {
      Serial.println("connected");
     
      if(readBMEdataCheck()) publishData();
      
 
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    if (attempt++>MAXATTEMPT) break;
   }  
   
   
   }

 bool readBMEdataCheck()
 {      
      unsigned int chkCount=0;
      BMEstatus = bme.begin();
    //if (!BMEstatus)
    while(!BMEstatus && chkCount < 3) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        delay(5000);
        BMEstatus = bme.begin();
        chkCount++;
        //while (1);
    }
    if (chkCount==3) return false;
    else return true;
    
   }

     void publishData(){
      String temp=String(bme.readTemperature());
      temp.toCharArray(tempData,20);
      client.publish("IoT/NodeMCU_01/BME280/temp",tempData,true); 
      String hum=String( bme.readHumidity());
      hum.toCharArray(humData,20);
      client.publish("IoT/NodeMCU_01/BME280/hum",humData,true); 
      String alt=String(bme.readAltitude(SEALEVELPRESSURE_HPA));
      alt.toCharArray(altData,20);
      client.publish("IoT/NodeMCU_01/BME280/alt",altData,true); 
      String pres=String(bme.readPressure()/100.0);
      pres.toCharArray(presData,20);
      client.publish("IoT/NodeMCU_01/BME280/pres",presData,true); 
     
     }
     
  
  
   

