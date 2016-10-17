/**
  Sensor Networks Example Arduino Xively Cloud Storage
  
  This project demonstrates how to save sensor data to an Xively feed.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>

// Ethernet shield Mac address
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Your Xively key
char xivelyKey[] = "<YOUR_KEY_HERE>";

// Your feed number
#define FEED_NUMBER <YOUR_FEED_HERE>

// Pin number for TMP36
#define SENSOR_PIN 0

// Define the strings for our datastream IDs
char sensor1_name[] = "celsius";
char sensor2_name[] = "fahrenheit";
XivelyDatastream datastreams[] = {
  XivelyDatastream(sensor1_name, strlen(sensor1_name), DATASTREAM_FLOAT),
  XivelyDatastream(sensor2_name, strlen(sensor2_name), DATASTREAM_FLOAT),
};

// Finally, wrap the datastreams into a feed
XivelyFeed feed(FEED_NUMBER, datastreams, 2 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);

void get_temperature(float *tempc, float *tempf) {
  int adc_data = analogRead(SENSOR_PIN);
  
  Serial.print("Temperature is ");
  *tempc = ((adc_data * 1200.0 / 1024.0) - 500.0) / 10.0;
  Serial.print(*tempc);
  Serial.print("c, ");
  *tempf = ((*tempc * 9.0)/5.0) + 32.0;
  Serial.print(*tempf);
  Serial.println("f");
  
  // wait for 1 second
  delay(1000); 
}

void record_sample(float tempc, float tempf) {
  datastreams[0].setFloat(tempc);
  datastreams[1].setFloat(tempf);

  Serial.print("Uploading to Xively ... ");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("done. Return code = ");
  Serial.println(ret);
  
  // Wait for data upload
  delay(15000);
}

void setup() {
  Serial.begin(9600);
  
  Serial.println("Starting cloud data upload.");

  while (!Ethernet.begin(mac_addr)) {
    Serial.println("Error starting Ethernet, trying again.");
    delay(1000);
  }
  
}

void loop() {
  float temp_celsius;
  float temp_fahrenheit;
  
  // read and record sensor data
  get_temperature(&temp_celsius, &temp_fahrenheit);
  record_sample(temp_celsius, temp_fahrenheit);
}
