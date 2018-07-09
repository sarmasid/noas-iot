#include <XBee.h>
XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x416C44C1); //destAddr);
//ZBTxRequest zbTx = ZBTxRequest(addr64, payload, payloadLength);
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

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
  dht_t dhtData;
  battery_t batteryData;
  rssi_t rssiData;
};
typedef union XBee_Packet_t{
  sensorData_t sensData;
  uint8_t xbeePacket[sizeof(sensorData_t)];
};

//XBee_Packet_t xbeepkt, readxbeepkt;




int Xbsend(XBee_Packet_t xbp){ 
int xbpl; //packet length
xbpl = sizeof(sensorData_t);
uint8_t payload[xbpl]; 
//Serial.print(xbp.sensData.senseParam);
swSerial.print(F("Frame # "));
swSerial.println(xbp.sensData.frameNo);
swSerial.println(xbp.sensData.bmeData.bme280TempVal);

 for(int i=0; i<xbpl; i++){
      payload[i] = xbp.xbeePacket[i];
      //Serial.println(payload[i], HEX);
 }

/*-----this part is taken from A. Rapp's Series2_Tx example -----*/
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x416C44C1); //destAddr);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
//ZBTxStatusResponse txStatus = ZBTxStatusResponse();

//Serial.print(F("Frame id of current packet: "));
//Serial.println(zbTx.getFrameId());
/*----send the packet-------------*/
xbee.send(zbTx);

/*----here it receives the reponse------------*/
 if (xbee.readPacket(500)) {
  if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        return 0;
      }else if (xbee.getResponse().isError()) {
        return xbee.getResponse().getErrorCode();
      }else{
        swSerial.println(F("unknown error!"));
        return 42; // just an arbitary value
      }
 }else{
  swSerial.println(F("No response received!"));
  return 59;
 }
}else{
  swSerial.println(F("No packet received!"));
  return 99; // just an arbitary value
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
