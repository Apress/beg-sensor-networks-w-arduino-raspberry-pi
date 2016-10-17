# RasPi XBee Data Aggregator - Beginning Sensor Networks
# 
# For this script, we read data from an XBee coordinator
# node whenever data is received from an XBee sensor node.
#
# The data read is from one sample (temperature from a
# TMP36 sensor and the supply voltage at the source.

#import pprint
import serial
import xbee

SERIAL_PORT = '/dev/ttyAMA0'
BAUD_RATE = 9600

# Instantiate an instance for the serial port
ser_port = serial.Serial(SERIAL_PORT, BAUD_RATE)

# Instantiate an instance of the ZigBee class
# and pass it an instance of the Serial class
xbee1 = xbee.zigbee.ZigBee(ser_port)

# Read and display temperature data
while True:
    try:
        # Read a data frame from the XBee
        data_samples = xbee1.wait_read_frame()
        #pprint.pprint(data_samples)

        # Get the address of the source XBee
        address = data_samples['source_addr_long']

        print("Reading from XBee:")
        samples = data_samples['samples'][0]

        # Calculate temperature in Celsius
        temp_c = ((samples['adc-3'] * 1200.0 / 1024.0) - 500.0) / 10.0

        # Calculate temperature in Fahrenheit
        temp_f = ((temp_c * 9.0) / 5.0) + 32.0

        # Display the temperature in Celsius and Fahrenheit
        print("  Temperature is {0} c, "
              "{1} f".format(temp_c, temp_f))

        # Calculate supply voltage
        volts = (float(samples['adc-7']) * (1200.0 / 1024.0)) / 1000.0

        # Display the supply voltage
        print("  Supply voltage = {0} volts.".format(volts))

    # Catch keyboard interrupt (CTRL-C) keypress
    except KeyboardInterrupt:
        break
        
# Close the port
ser_port.close()
