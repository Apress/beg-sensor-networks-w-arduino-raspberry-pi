/*
  Sensor Networks Example Arduino Hosted Sensor Node

  This sensor node uses a DHT22 sensor to read temperature and humidity
  printing the results in the serial monitor.

*/
#include <SPI.h>
#include <DHT22.h>

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
      sprintf(buf, “%hi.%01hi, %i.%01i\n",
                   myDHT22.getTemperatureCInt()/10,
                   abs(myDHT22.getTemperatureCInt()%10),
                   myDHT22.getHumidityInt()/10,
                   myDHT22.getHumidityInt()%10);
      Serial.print("Data read:");
      Serial.print(buf);
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
  Serial.begin(115200);  // Set the serial port speed
}

void loop() {  
  delay(read_delay);
  read_data();
}

