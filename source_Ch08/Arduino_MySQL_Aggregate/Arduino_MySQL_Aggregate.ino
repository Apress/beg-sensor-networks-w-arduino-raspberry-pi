/**
  Sensor Networks Example Arduino Data Aggregate Node
  
  This project demonstrates how to receive sensor data from
  multiple XBee sensor nodes saving the samples in a MySQL
  database.

  It uses an Arduino with an XBee shield with an XBee 
  coordinator installed.
  
*/

#include <XBee.h>
#include <SoftwareSerial.h>
#include <Ethernet.h>
#include <SPI.h>
#include <sha1.h>
#include <mysql.h>

// Setup pin definitions for XBee shield
uint8_t recv = 2;
uint8_t trans = 3;
SoftwareSerial soft_serial(recv, trans);

// assign a MAC address and IP address for the Arduino
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(10, 0, 1, 111);
IPAddress server_addr(10, 0, 1, 24); 

// Instantiate an instance of the XBee library
XBee xbee = XBee();

// Instantiate an instance of the IO sample class
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

/* Setup for the Connector/Arduino */
Connector my_conn;        // The Connector/Arduino reference

char user[] = "root";
char password[] = "root";

// Sample data values
unsigned int address;        // Last 4 digits of XBee address
float temperature;  // Raw temperature value
float voltage;      // Reference voltage

// Get sample data
void get_sample_data(ZBRxIoSampleResponse *ioSample) {
  Serial.print("Received data from address: ");
  address = (ioSample->getRemoteAddress64().getMsb() << 8) + 
             ioSample->getRemoteAddress64().getLsb();
  Serial.print(ioSample->getRemoteAddress64().getMsb(), HEX);  
  Serial.println(ioSample->getRemoteAddress64().getLsb(), HEX);  
  temperature = ioSample->getAnalog(3);
  int ref = xbee.getResponse().getFrameData()[17] << 8;
  ref += xbee.getResponse().getFrameData()[18];
  voltage = (float(ref) * float(1200.0 / 1024.0))/1000.0;
}


// Record a sample 
void record_sample(ZBRxIoSampleResponse *ioSample) {
  int saved_addr;

  // Get sample data from XBee
  get_sample_data(ioSample);
  
  // Send data to MySQL
  String query("INSERT INTO house.temperature VALUES(NULL, '");
  String addr(address, HEX);
  char buff[40];
  query += addr;
  query += ("', '");
  query += dtostrf(temperature, 4, 4, buff);
  query += ("', '");
  query += dtostrf(voltage, 4, 4, buff);
  query += ("', NULL, NULL)");
  
  Serial.println(&query[0]);
  my_conn.cmd_query(&query[0]);
}

void setup() { 
  // start the SPI library:
  SPI.begin();

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  while (!Serial); // Wait until Serial is ready - Leonardo
  soft_serial.begin(9600);
  xbee.setSerial(soft_serial);
  
  Serial.println("Connecting to MySQL...");
  if (my_conn.mysql_connect(server_addr, 3306, user, password))
  {
    delay(500);
    Serial.println("Success!");
  }
  else
  {
    Serial.println("Connection failed.");
  }
}

void loop() {
  //attempt to read a packet    
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      
      // Get the packet 
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);
      
      // Get and store the data locally (in memory or on card?)
      record_sample(&ioSample);
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
