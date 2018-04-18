from xbee import XBee 
import serial

import time myport = '/dev/ttyUSB0'
print myport


ser = serial.Serial(myport, 115200)

xbee = XBee(ser)
while True:
    # topic = ""
    try:
        response = xbee.wait_read_frame()
        sa = response['source_addr_long'] #[4:]
        rf = response['rf_data']
        print time.strftime('%x %X %Z'),' ',sa,' ',rf
   except KeyboardInterrupt:
        break

ser.close()