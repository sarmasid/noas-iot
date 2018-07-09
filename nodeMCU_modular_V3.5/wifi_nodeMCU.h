#include<WiFiClient.h>
#include<PubSubClient.h>
#define wifiAttemptMaxCount 10 //max number of attempts to connect to WiFi ssid
#define MQTTMAXATTEMPT 5 //max number of attemps to connect to mqtt broker
#define readDuration 10000
//#define MQTT_KEEPALIVE 60
char node_id[] = "NodeMCU_055";
String topicPrefix =String("IoT/" + String(node_id));
char topicFull[30];
char currentTime[30]=" ";
/*----wifi connection setup function----------------*/
bool wifi_init(char *ssid, char *pass){
  WiFi.begin(ssid,pass);
  unsigned int wifiAttemptCount = 0;
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
    if(wifiAttemptCount++>wifiAttemptMaxCount){
      return 0;
    }
  }
  Serial.println();
  Serial.print("Connected to Access Point :");
  Serial.println(ssid);
  Serial.print("IP address :: ");
  Serial.println(WiFi.localIP());
  return 1;
}


int reconnectMCU(PubSubClient client, const char *node_id) {
int attempt = 1;
//int rc =0;
//Serial.println(node_id);
   while (!client.connected()) {
       Serial.print("Attempting MQTT connection...");
      if (client.connect(node_id)) {
      Serial.println("connected");
      
      client.subscribe("IoTtimeStamp/");
      } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(10000);
    }
    if (attempt++>MQTTMAXATTEMPT) break;
   }
   return client.state();
}

/*-------- function to create a topic----------------------*/
String createTopic(char sensorName[10], char sensorType[5])
{
  return String(topicPrefix +"/"+String(sensorName)+"/"+String(sensorType));
}

/*---------------function to convert string to publishable character array----------------*/
void str2CharArray(String strObj, char charArr[],unsigned int len)
{
 strObj.toCharArray(charArr,len);
}

/*------------function to publish data-----------------------*/
void publishTopic(PubSubClient client, char *sensorName, char *senseType, char *dataToPublish){
      str2CharArray(createTopic(sensorName,senseType),topicFull,30);
      if(client.publish(topicFull,dataToPublish,true)){
       Serial.println("Data Published");
      }else{
        Serial.println("Failded to publish data"); 
      }
      delay(1000);
}

/*------------function for reading the incoming message----------------*/ 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    currentTime[i]=(char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/*---------------function to read the time published under timestamp-----------*/
 void readCurrTime(PubSubClient client){
  long timerStart, timeNow;
 timerStart=millis();
  while(timeNow-timerStart<readDuration){
   client.loop();
   timeNow = millis();
  }
 }
 

