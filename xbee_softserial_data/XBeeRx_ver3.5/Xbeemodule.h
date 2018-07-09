#include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
  ZBRxResponse rx = ZBRxResponse();
  ModemStatusResponse msr = ModemStatusResponse();

uint8_t dbCmd[2] = {'D','B'}; 
typedef union dbCmd_Rsp_t{
int rssi;
uint8_t dbCmdRsp[sizeof(int)];
} dbcmdRsp;

typedef struct ds18b20_t{
  char ds18b20code;
  float ds18b20TempVal;
};
typedef struct bme280_t{
  char bme280code;
  float bme280TempVal;
  float bme280HumVal;
  float bme280PresVal;
};
typedef struct dht_t{
  char dhtcode;
  float dhtTempVal;
  float dhtHumVal;
};

typedef struct lm35_t{
  char lm35code;
  float lm35TempVal;
};

typedef struct battery_t{
  char batterycode;
  float batteryVoltVal;
};

typedef struct rssi_t{
  char rssicode;
  int rssiVal;
};

typedef struct sensorData_t{
  long frameNo;
  ds18b20_t ds18b20Data;
  bme280_t bmeData;  
  lm35_t lm35Data;
  battery_t batteryData;
  dht_t dhtData;
  rssi_t rssiData;
  
};
typedef union XBee_Packet_t{
  sensorData_t sensData;
  uint8_t xbeePacket[sizeof(sensorData_t)];
};

int Xbrecv(unsigned int timeout, XBee_Packet_t *rxbp, uint16_t *dl, uint16_t *pl){
       xbee.readPacket(timeout);
    
    if (xbee.getResponse().isAvailable()) {
      // got something
           
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet
        
        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx);
      
        //Serial.println("Got an rx packet!");
        //Serial.print(F("Frame id: "));
        //Serial.println( rx.getFrameId(), HEX );    
        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
            // the sender got an ACK
            //Serial.println("packet acknowledged");
        } else {
          Serial.println("packet not acknowledged");
        }
        

        *pl = rx.getPacketLength(); //packet length
        *dl = rx.getDataLength();   // data length
        Serial.print(F("16 bit address: 0x"));
        Serial.println(rx.getRemoteAddress16(),HEX);
        XBeeAddress64 raddr64 = rx.getRemoteAddress64();
        Serial.print(F("64 bit address: 0x"));
        Serial.print(raddr64.getMsb(),HEX);
        Serial.println(raddr64.getLsb(), HEX);
        
      for (int i = 0; i < rx.getDataLength(); i++) {
        rxbp->xbeePacket[i]=rx.getData()[i];
       // Serial.println(rxbp->xbeePacket[i], HEX);
       }
         
      return 0;
      }
      return 55;
    } else if (xbee.getResponse().isError()) {
      Serial.print("error code:");
      Serial.println(xbee.getResponse().getErrorCode());
      return xbee.getResponse().getErrorCode();
    }
}



void sendAtCommand(uint8_t *cmd, dbCmd_Rsp_t *dbRsp ) {
 // Serial.println("Sending command to the XBee");
  AtCommandRequest atRequest = AtCommandRequest(cmd);
  AtCommandResponse atResponse = AtCommandResponse();
  // send the command
  xbee.send(atRequest);

  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000)) {
    // got a response!

    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);

      if (atResponse.isOk()) {
//        Serial.print("Command [");
//        Serial.print(atResponse.getCommand()[0]);
//        Serial.print(atResponse.getCommand()[1]);
//        Serial.println("] was successful!");

        if (atResponse.getValueLength() > 0) {
//          Serial.print("Command value length is ");
//          Serial.println(atResponse.getValueLength(), DEC);
//
//          Serial.print("Command value: ");
      //    cmdRsp = atResponse.getValue();
          for (int i = 0; i < atResponse.getValueLength(); i++) {
           // Serial.print(atResponse.getValue()[i], HEX);
           // Serial.print(" ");
           dbRsp->dbCmdRsp[i] = atResponse.getValue()[i];
          }

          //Serial.println("");
        }
      } 
      else {
        Serial.print("Command return error code: ");
        Serial.println(atResponse.getStatus(), HEX);
      }
    } else {
      Serial.print("Expected AT response but got ");
      Serial.print(xbee.getResponse().getApiId(), HEX);
    }   
  } else {
    // at command failed
    if (xbee.getResponse().isError()) {
      Serial.print("Error reading packet.  Error code: ");  
      Serial.println(xbee.getResponse().getErrorCode());
    } 
    else {
      Serial.print("No response from radio");  
    }
  }
}



