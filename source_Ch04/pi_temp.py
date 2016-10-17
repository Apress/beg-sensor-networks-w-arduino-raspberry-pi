# RasPi Temperature Sensor - Beginning Sensor Networks
# 
# For this script, we explore connecting a digital temperature sensor
# to the Raspberry Pi and reading the data. We display the temperature
# in Celsius and Fahrenheit.

# Import Python modules (always list in alphabetical order)
import glob
import os
import time

# Issue the modprobe statements to initialize the GPIO and
# temperature sensor modules
os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

# Use glob to search the file system for files that match the prefix.
# Save the directory to the file.
datadir = glob.glob('/sys/bus/w1/devices/28*')[0]
# Create the full path to the file
datafile = os.path.join(datadir, '/w1_slave')

# Procedure for reading the raw data from the file.
# Open the file and read all of the lines then close it.
def read_data():
    f = open(datafile, 'r')
    lines = f.readlines()
    f.close()
    return lines

# Read the temperature and return the values found.
def get_temp():
    # Initialize the variables.
    temp_c = None
    temp_f = None
    lines = read_data()

    # If the end of the first line ends with something other than 'YES'
    # Try reading the file again until 'YES' is found.
    while not lines[0].strip().endswith('YES'):
        time.sleep(0.25)
        lines = read_data()
    
    # Search the second line for the data prefixed with 't='   
    pos = lines[1].find('t=')

    # A return code of -1 means it wasn't found.
    if pos != -1:

        # Get the raw data located after the 't=' until the end of the line.
        temp_string = lines[1][pos+2:]

        # Convert the scale for printing
        temp_c = float(temp_string) / 1000.00

        # Convert to Farenheit
        temp_f = temp_c * 9.00 / 5.00 + 32.00

    # Return the values read
    return temp_c, temp_f

# Main loop. Read data then sleep 1 second until cancelled with CTRL-C.
while True:
    temp_c, temp_f = get_temp()
    print("Temperature is {0} degrees Celsius, "
          "{1} degrees Fahrenheit.".format(temp_c, temp_f))
    time.sleep(1)
