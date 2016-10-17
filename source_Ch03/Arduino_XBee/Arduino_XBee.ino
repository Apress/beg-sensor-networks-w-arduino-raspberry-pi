/**
  Sensor Networks Example Arduino Receiver Node
  
  This project demonstrates how to receive sensor data from
  an XBee sensor node. It uses an Arduino with an XBee shield
  with an XBee coordinator installed.
  
  Note: This sketch was adapted from the examples in the XBee 
  library created by Andrew Rapp.
*/

#include <XBee.h>
#include <SoftwareSerial.h>

// Setup pin definitions for XBee shield
uint8_t recv = 2;
uint8_t trans = 3;
SoftwareSerial soft_serial(recv, trans);

// Instantiate an instance of the XBee library
XBee xbee = XBee();

// Instantiate an instance of the IO sample class
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

void setup() { 
  Serial.begin(9600);
  while (!Serial);   // Leonardo boards need to wait for Serial to start
  soft_serial.begin(9600);
  xbee.setSerial(soft_serial);
}

// Get address and print it
void get_address(ZBRxIoSampleResponse *ioSample) {
  Serial.print("Received data from address: ");
  Serial.print(ioSample->getRemoteAddress64().getMsb(), HEX);  
  Serial.print(ioSample->getRemoteAddress64().getLsb(), HEX);  
  Serial.println("");
}

// Get temperature and print it
void get_temperature(ZBRxIoSampleResponse *ioSample) {
  float adc_data = ioSample->getAnalog(3);

  Serial.print("Temperature is ");
  float temperatureC = ((adc_data * 1200.0 / 1024.0) - 500.0) / 10.0;
  Serial.print(temperatureC);
  Serial.print("c, ");
  float temperatureF = ((temperatureC * 9.0)/5.0) + 32.0;
  Serial.print(temperatureF);
  Serial.println("f");
}

// Get supply voltage and print it
void get_supply_voltage() {
  Serial.print("Supply voltage is ");
  int ref = xbee.getResponse().getFrameData()[17] << 8;
  ref += xbee.getResponse().getFrameData()[18];
  float volts = (float(ref) * float(1200.0 / 1024.0))/1000.0;
  Serial.print(" = ");
  Serial.print(volts);
  Serial.println(" volts.");
}

void loop() {
  //attempt to read a packet    
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      
      // Get the packet 
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);

      // Read and display data
      get_address(&ioSample);
      get_temperature(&ioSample);
      get_supply_voltage();
    } 
    else {
      Serial.print("Expected I/O Sample, but got ");
      Serial.print(xbee.getResponse().getApiId(), HEX);
    }    
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error reading packet.  Error code: ");  
    Serial.println(xbee.getResponse().getErrorCode());
  }
}
