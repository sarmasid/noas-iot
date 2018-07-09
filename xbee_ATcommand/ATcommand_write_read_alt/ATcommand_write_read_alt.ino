/* -------------------------------------------------------------
This code is written as a part of IIT Mandi IoT testbed project. 
I/We thank all the authors/organizations for providing libraries for different modules. 
*****************************************************************************
* last update: July 8, 2018 by Siddhartha Sarma (sarma.siddhartha@yahoo.com)
*/
#include <XBee.h>
#include <SoftwareSerial.h>


uint8_t ssRx = 8;
uint8_t ssTx = 9;
SoftwareSerial swSerial(ssRx, ssTx);
XBee xbee = XBee();
uint8_t niCmd[] = {'N','I'};
uint8_t niValue[] = {"ABCDEF"}; 
uint8_t wrCmd[] = {'W','R'}; 
uint8_t *paramVal;

//AtCommandRequest setRequest = AtCommandRequest(niCmd, niValue, sizeof(niValue));
AtCommandRequest writeRequest = AtCommandRequest(wrCmd);
//AtCommandRequest readRequest = AtCommandRequest(niCmd);
AtCommandResponse atResponse = AtCommandResponse();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  swSerial.begin(9600);
  xbee.begin(swSerial);
  delay(5000);
  boolean atcmdstatus = setXbeeParamVal(niCmd, niValue);
  if(atcmdstatus) Serial.println("Success!");
  delay(5000);
  getXbeeParamVal(niCmd, paramVal);
  delay(5000);
  //sendAtCommand(readRequest);
}

void loop() {
  // put your main code here, to run repeatedly:
   
}
boolean setXbeeParamVal(uint8_t *cmd, uint8_t *cmdVal){
 uint8_t Rsp[] = {'0','0'};
 AtCommandRequest setRequest = AtCommandRequest(cmd, cmdVal, sizeof(cmdVal));
 xbee.send(setRequest);
 if(getXbeeATresponse(Rsp)==0){
  xbee.send(writeRequest);
  if (getXbeeATresponse(Rsp)==0){
    return 1;
  }
 }else return 0;
}

int getXbeeParamVal(uint8_t *cmd, uint8_t *cmdRsp){
 AtCommandRequest setRequest = AtCommandRequest(cmd);
 xbee.send(setRequest);
 return getXbeeATresponse(cmdRsp);
}
//void sendAtCommand(AtCommandRequest request){
//    xbee.send(request);

int getXbeeATresponse(uint8_t *cmdResponse){
  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000)) {
    // got a response!

    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);

      if (atResponse.isOk()) {
        Serial.print("Command [");
        Serial.print(atResponse.getCommand()[0]);
        Serial.print(atResponse.getCommand()[1]);
        Serial.println("] was successful!");

        if (atResponse.getValueLength() > 0) {
          Serial.print("Command value length is ");
          Serial.println(atResponse.getValueLength(), DEC);

          Serial.print("Command value: ");
          //cmdRsp = atResponse.getValue();
          for (int i = 0; i < atResponse.getValueLength(); i++) {
            Serial.print(atResponse.getValue()[i], HEX);
            Serial.print(" ");
            cmdResponse[i] = atResponse.getValue()[i];
          }

          Serial.println("");
        }
        return 0;
      } 
      else {
        swSerial.print("Command return error code: ");
        swSerial.println(atResponse.getStatus(), HEX);
        return 1;
      }
    } else {
      swSerial.print("Expected AT response but got ");
      swSerial.print(xbee.getResponse().getApiId(), HEX);
      return 2;
    }   
  } else {
    // at command failed
    if (xbee.getResponse().isError()) {
      swSerial.print("Error reading packet.  Error code: ");  
      swSerial.println(xbee.getResponse().getErrorCode());
      return 3;
    } 
    else {
      swSerial.print("No response from radio");  
      return 4;
    }
  }
}
