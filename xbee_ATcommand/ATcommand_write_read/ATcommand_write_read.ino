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
uint8_t niValue[] = {"ENDNODE"}; 
uint8_t wrCmd[] = {'W','R'}; 

AtCommandRequest setRequest = AtCommandRequest(niCmd, niValue, sizeof(niValue));
AtCommandRequest writeRequest = AtCommandRequest(wrCmd);
AtCommandRequest readRequest = AtCommandRequest(niCmd);
AtCommandResponse atResponse = AtCommandResponse();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  swSerial.begin(9600);
  xbee.begin(swSerial);
  delay(5000);
  sendAtCommand(setRequest);
  delay(5000);
  sendAtCommand(writeRequest);
  delay(5000);
  sendAtCommand(readRequest);
}

void loop() {
  // put your main code here, to run repeatedly:
   
}

void sendAtCommand(AtCommandRequest request){
    xbee.send(request);

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
          // dbRsp->dbCmdRsp[i] = atResponse.getValue()[i];
          }

          Serial.println("");
        }
      } 
      else {
        swSerial.print("Command return error code: ");
        swSerial.println(atResponse.getStatus(), HEX);
      }
    } else {
      swSerial.print("Expected AT response but got ");
      swSerial.print(xbee.getResponse().getApiId(), HEX);
    }   
  } else {
    // at command failed
    if (xbee.getResponse().isError()) {
      swSerial.print("Error reading packet.  Error code: ");  
      swSerial.println(xbee.getResponse().getErrorCode());
    } 
    else {
      swSerial.print("No response from radio");  
    }
  }
}
