/**
  Sensor Networks Example Arduino DHT22 Sensor
  
  This project demonstrates how to receive sensor data from
  a DHT22 sensor saving the samples in a MySQL
  database.

  It uses an Arduino with an Ethernet shield installed.
*/
#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
#include "mysql.h"
#include <DHT22.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server_addr(10, 0, 0, 24); 
char user[] = "root";
char password[] = "secret";

Connector my_conn;        // The Connector/Arduino reference

#define DHT22_PIN 7       // DHT2 data is on pin 7
#define read_delay 5000   // 5 seconds 
DHT22 myDHT22(DHT22_PIN); // DHT22 instance

void read_data() {
  DHT22_ERROR_t errorCode;
  
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      char buf[128];
      sprintf(buf, "INSERT INTO dht22_test.temp_humid VALUES (NULL, %hi.%01hi, %i.%01i)",
                   myDHT22.getTemperatureCInt()/10,
                   abs(myDHT22.getTemperatureCInt()%10),
                   myDHT22.getHumidityInt()/10,
                   myDHT22.getHumidityInt()%10);
      my_conn.cmd_query(buf);
      Serial.println("Data read and recorded.");
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.print("check sum error ");
      Serial.print(myDHT22.getTemperatureC());
      Serial.print("C ");
      Serial.print(myDHT22.getHumidity());
      Serial.println("%");
      break;
    case DHT_BUS_HUNG:
      Serial.println("BUS Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Polled too quick ");
      break;
  }
}

void setup() {  
  Ethernet.begin(mac_addr);
  Serial.begin(115200);
  delay(1000);
  Serial.println("Connecting...");
  if (my_conn.mysql_connect(server_addr, 3306, user, password))
    delay(500);
  else
    Serial.println("Connection failed.");
}

void loop() {  
  delay(read_delay);
  read_data();
}

