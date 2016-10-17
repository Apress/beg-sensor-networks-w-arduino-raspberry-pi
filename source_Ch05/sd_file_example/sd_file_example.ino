/**
  Sensor Networks Example Arduino SD card data store
  
  This project demonstrates how to save sensor data to a 
  microSD card.
*/

#include <Wire.h>
#include <RTClib.h>
#include <SD.h>
#include <String.h>

// Pin assignment for Arduino Ethernet shield
#define SD_PIN 4
// Pin assignment for Sparkfun microSD shield
//#define SD_PIN 8
// Pin assignment for Adafruit Data Logging shield 
//#define SD_PIN 10

// Sensor data file - require 8.3 file name
#define SENSOR_DATA "sensdata.txt"

RTC_DS1307 RTC;
File sensor_data;

void record_sample(int data) {
  // Open the file
  sensor_data = SD.open(SENSOR_DATA, FILE_WRITE);
  if (!sensor_data) {
    Serial.println("ERROR: Cannot open file. Data not saved!");
    return;
  }
  
  // Capture the date and time
  DateTime now = RTC.now();

  String timestamp(now.month(), DEC);
  timestamp += ("/");
  timestamp += now.day();
  timestamp += ("/");
  timestamp += now.year();
  timestamp += (" ");
  timestamp += now.hour();
  timestamp += (":");
  timestamp += now.minute();
  timestamp += (":");
  timestamp += now.second();
  timestamp += (" ");
 
  // Save the sensor data
  sensor_data.write(&timestamp[0]);
  
  String sample(data, DEC);
  sensor_data.write(&sample[0]);
  sensor_data.write("\n");
  
  // Echo the data
  Serial.print("Sample: ");
  Serial.print(timestamp);
  Serial.print(data, DEC);
  Serial.println();
  
  // Close the file
  sensor_data.close();
}

void setup () {
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  if (!RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // Set time to date and time of compilation
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  // disable w5100 SPI
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);

  // Initialize the SD card.  
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // Check for file. Create if not present
  if (!SD.exists(SENSOR_DATA)) {
    Serial.print("Sensor data file does not exit. Creating file...");
    sensor_data = SD.open(SENSOR_DATA, FILE_WRITE);
    if (!sensor_data) {
      Serial.println("ERROR: Cannot create file.");
    }
    else {
      sensor_data.close();
      Serial.println("done.");
    }
  }
  
  // Record some test samples.
  record_sample(1);
  record_sample(2);
  record_sample(3);
}

void loop () {
  // Read sensor data here and record with record_sample()
}


