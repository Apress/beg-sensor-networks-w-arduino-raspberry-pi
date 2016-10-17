/**
  Sensor Networks Example Arduino External EEPROM data store
  
  This project demonstrates how to save and retrieve sensor data
  to/from an external EEPROM chip.
*/

#include <Wire.h>    

#define FIRST_SAMPLE 0x02 // First position of fist sample
#define MEM_ADDR 0x50     // EEPROM address
#define BUTTON_PIN 0x02   // Button pin
#define EEPROM_SIZE 32768 // Size of 24LC256
#define SAMPLE_BYTES 2    // Size of sample in bytes

int next_index = 0;       // Address of first sample

/* Initialize the chip erasing data */
void initialize(int address) {
  // Clear memory 
  // NOTE: replace '10' with EEPROM_SIZE to erase all data
  for (int i = 0; i < 10; i++) {
    write_byte(address, i, 0xFF);
  }
  write_byte(address, 0, FIRST_SAMPLE);
  write_byte(address, 1, SAMPLE_BYTES);
  Serial.print("EEPROM at address 0x");
  Serial.print(address, HEX);
  Serial.println(" has been initialized.");
}

/* Write a sample to the chip. */
void write_sample(int address, unsigned int index, byte *data) {
  Wire.beginTransmission(address);
  Wire.write((int)(index >> 8));   // MSB
  Wire.write((int)(index & 0xFF)); // LSB
  Serial.print("START: ");
  Serial.println(index);
  for (int i = 0; i < SAMPLE_BYTES; i++) {
    Wire.write(data[i]);
  }
  Wire.endTransmission();

  delay(5); // wait for chip to write data
}

/* Write a byte to the chip at specific index (offset). */
void write_byte(int address, unsigned int index, byte data) {
  Wire.beginTransmission(address);
  Wire.write((int)(index >> 8));   // MSB
  Wire.write((int)(index & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
 
  delay(5);
}
 
/* Read a sample from an index (offset). */
void read_sample(int address, unsigned int index, byte *buffer) {  
  Wire.beginTransmission(address);
  Wire.write((int)(index >> 8));   // MSB
  Wire.write((int)(index & 0xFF)); // LSB
  Wire.endTransmission();
  
  Wire.requestFrom(address, SAMPLE_BYTES);
  for (int i = 0; i < SAMPLE_BYTES; i++) {
    if (Wire.available()) {
      buffer[i] = Wire.read();
    }
  }
}

/* Read a byte from an index (offset). */
byte read_byte(int address, unsigned int index)  {
  byte data = 0xFF;
 
  Wire.beginTransmission(address);
  Wire.write((int)(index >> 8));   // MSB
  Wire.write((int)(index & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(address,1);
 
  if (Wire.available()) {
    data = Wire.read();
  }
  return data;
}

/* Save a sample to the data chip and increment next address counter. */
void record_sample(int address, int data) {
  byte sample[SAMPLE_BYTES];
  
  sample[0] = data >> 8;
  sample[1] = (byte)data;
  write_sample(address, next_index, sample);
  next_index += SAMPLE_BYTES;
  write_byte(address, 0, next_index);
}

void setup(void) {
  int bytes_per_sample = SAMPLE_BYTES;
  byte buffer[SAMPLE_BYTES];
  
  Serial.begin(115200);
  Wire.begin();  
  
  next_index = read_byte(MEM_ADDR, 0);
  bytes_per_sample = read_byte(MEM_ADDR, 1);
  Serial.println("Welcome to the Arduino external EEPROM project.");
  Serial.print("Byte pointer: ");
  Serial.println(next_index, DEC);
  Serial.print("Bytes per sample: ");
  Serial.println(bytes_per_sample, DEC);
  Serial.print("Number of samples:");
  Serial.println((next_index/bytes_per_sample)-1, DEC);

  // Add some sample data
  record_sample(MEM_ADDR, 6011);
  record_sample(MEM_ADDR, 8088);
  
  // Example of how to read sample data - read last 2 values
  read_sample(MEM_ADDR, next_index-(SAMPLE_BYTES * 2), buffer);
  Serial.print("First value: ");
  Serial.println((int)(buffer[0] << 8) + (int)buffer[1]);
  read_sample(MEM_ADDR, next_index-SAMPLE_BYTES, buffer);
  Serial.print("Second value: ");
  Serial.println((int)(buffer[0] << 8) + (int)buffer[1]);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {     
    initialize(MEM_ADDR);
    delay(500); // debounce
  }
  //
  // Read sensor data and record sample here
  //
}
 

