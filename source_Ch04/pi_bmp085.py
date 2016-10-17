# RasPi I2C Sensor - Beginning Sensor Networks
# 
# For this script, we connect to and read data from an
# I2C sensor. We use the BMP085 sensor module from Adafruit
# or Sparkfun to read barometric pressure and altitude
# using the Adafruit I2C Python code.

import time

# Import the BMP085 class
from Adafruit_BMP085 import BMP085

# Instantiate an instance of the BMP085 class passing
# the address of the sensor as displayed by i2cdetect.
bmp085 = BMP085(0x77)

# Read data until cancelled
while True:
    try:
        # Read the data
        pressure = float(bmp085.readPressure()) / 100
        altitude = bmp085.readAltitude()

        # Display the data
        print("The barometric pressure at altitude {0:.2f} "
              "is {1:.2f} hPa.".format(pressure, altitude))

        # Wait for a bit to allow sensor to stabilize
        time.sleep(3)

    # Catch keyboard interrupt (CTRL-C) keypress
    except KeyboardInterrupt:
        break

