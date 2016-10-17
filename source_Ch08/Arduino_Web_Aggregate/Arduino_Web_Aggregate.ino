/**
  Sensor Networks Example Arduino Data Aggregate Node
  
  This project demonstrates how to receive sensor data from
  multiple XBee sensor nodes, save the samples in the onboard
  EEPROM and present them as a web page. It uses an Arduino 
  with an XBee shield with an XBee coordinator installed.
  
  Note: This sketch was adapted from the examples in the XBee 
  library created by Andrew Rapp.
*/

#include <XBee.h>
#include <SoftwareSerial.h>
#include <Ethernet.h>
#include <SPI.h>
#include <EEPROM.h>

byte bytes_per_sample = 10; // address (2), temp (4), volts (4)

// Setup pin definitions for XBee shield
uint8_t recv = 2;
uint8_t trans = 3;
SoftwareSerial soft_serial(recv, trans);

// assign a MAC address and IP address for the Arduino
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(10, 0, 1, 111);

// Start Ethernet Server class with port 80 (default for HTTP)
EthernetServer server(80);

// Instantiate an instance of the XBee library
XBee xbee = XBee();

// Instantiate an instance of the IO sample class
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

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

// Read an integer from EEPROM
int read_int(byte position) {
  int value = 0;
  byte* p = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(value); i++)
      *p++ = EEPROM.read(position++);
  return value;
}

// Read a float from EEPROM
float read_float(byte position) {
  float value = 0;
  byte* p = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(value); i++)
      *p++ = EEPROM.read(position++);
  return value;
}

// Write an integer to EEPROM
void write_int(byte position, int value) {
  const byte *p = (const byte *)(const void *)&value;
  for (int i = 0; i < sizeof(value); i++)
      EEPROM.write(position++, *p++);
}

// Write a float to EEPROM
void write_float(byte position, float value) {
  const byte *p = (const byte *)(const void *)&value;
  for (int i = 0; i < sizeof(value); i++)
      EEPROM.write(position++, *p++);
}

// Read a sample from EEPROM
void read_sample(byte index, unsigned int *address, float *temp_f, 
                 float *temp_c, float *volts) {
  float temp;
  byte position =  (index * bytes_per_sample) + 1;
  
  *address = read_int(position);
  
  temp = read_float(position + 2);
  
  *temp_c = ((temp * 1200.0 / 1024.0) - 500.0) / 10.0;
  *temp_f = ((*temp_c * 9.0)/5.0) + 32.0;

  *volts = read_float(position + 6);
}

// Write sample to EEPROM
void write_sample(byte index) {
  byte position =  (index * bytes_per_sample) + 1;

  write_int(position, address);
  write_float(position + 2, temperature);
  write_float(position + 6, voltage);
}

// Record a sample 
void record_sample(ZBRxIoSampleResponse *ioSample) {
  int saved_addr;

  // Get sample data from XBee
  get_sample_data(ioSample);
  
  // See if already in memory. If not, add it.
  byte num_samples = EEPROM.read(0);
  boolean found = false;
  for (byte i = 0; i < num_samples; i++) {
    byte position = (i * bytes_per_sample) + 1;
    
    // get address
    saved_addr = read_int(position);
    if (saved_addr == address) {
      write_sample(i);
      found = true;
    }
  }
  if (!found) {
    // Save sample    
    write_sample(num_samples);

    // Update number of sensors
    num_samples++;
    EEPROM.write(0, num_samples);
  }
}

void send_sensor_data(EthernetClient *client, int num_sample) {
  unsigned int address;
  float temp_c;
  float temp_f;
  float volts;

  // Read sensor data from memory and display it.
  read_sample(num_sample, &address, &temp_c, &temp_f, &volts);
  
  client->println("<br />");  
  client->print("Node Address: ");
  client->print(address, HEX);
  client->print(".");
  client->println("<br />");
  client->print("Temperature: ");
  client->print(temp_c);
  client->print(" degrees C");
  client->println("<br />");
  client->print("Temperature: ");
  client->print(temp_f);
  client->print(" degrees F");
  client->println("<br />");   
  client->print("Voltage: ");
  client->print(volts);
  client->print(" volts.");
  client->println("<br />");  
}

void listener() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Got a connection!");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          
          // Print header
          client.println("Welcome to the Arduino Data Aggregate Node!");
          client.println("<br />");  
          
          // read sensor data
          byte num_samples = EEPROM.read(0);
          for (int i = 0; i < num_samples; i++) {
            send_sensor_data(&client, i);
          }
          // if no data, say so!
          if (num_samples == 0) {
            client.print("No samples to display.");
            client.println("<br />");  
          }
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
} 

void setup() { 
  // start the SPI library:
  SPI.begin();

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.begin(9600);
  while (!Serial); // Wait until Serial is ready - Leonardo
  soft_serial.begin(9600);
  xbee.setSerial(soft_serial);
  
  // Initialize the EEPROM
  EEPROM.write(0, 0);
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
  } else {
    // Listen for client and respond.
    listener();
  }
}
