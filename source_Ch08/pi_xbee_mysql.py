# RasPi XBee Remote Storage Data Aggregator - Beginning Sensor Networks
#
# For this script, we read data from an XBee coordinator
# node whenever data is received from an XBee sensor node.
# We also need a connection to a database server for saving
# the results in a table.
#
# The data read is from one sample (temperature from a
# XBee sensor node and the supply voltage at the source).

import serial
import xbee
import mysql.connector

# Constants
SERIAL_PORT = '/dev/ttyAMA0'
BAUD_RATE = 9600
USER = 'root'
HOST_OR_IP = '10.0.1.24'
PORT = 3306
PASSWD = 'secret'

# Instantiate an instance for the serial port
ser_port = serial.Serial(SERIAL_PORT, BAUD_RATE)

# Instantiate an instance of the ZigBee class
# and pass it an instance of the Serial class
xbee1 = xbee.zigbee.ZigBee(ser_port)

# Variables for MySQL Connector/Python code
db_conn = None

def run_query(conn, query_str):
    results = None
    cur = conn.cursor(
        cursor_class=mysql.connector.cursor.MySQLCursorBufferedRaw)
    try:
        res = cur.execute(query_str)
    except mysql.connector.Error, e:
        cur.close()
        raise Exception("Query failed. " + e.__str__())
    try:
        results = cur.fetchall()
    except mysql.connector.errors.InterfaceError, e:
        if e.msg.lower() == "no result set to fetch from.":
            pass # This error means there were no results.
        else:    # otherwise, re-raise error
            raise e
    conn.commit()
    cur.close()
    return results

# Connect to database server
try:
    parameters = {
        'user': USER,
        'host': HOST_OR_IP,
        'port': PORT,
        'passwd': PASSWD,
        }
    db_conn = mysql.connector.connect(**parameters)
except mysql.connector.Error, e:
    raise Exception("ERROR: Cannot connect to MySQL Server!")

# Read and save temperature data
while True:
    try:
        # Read a data frame from the XBee
        data_samples = xbee1.wait_read_frame()

        # Get the address of the source XBee
        address = data_samples['source_addr_long']

        print("Reading from XBee:")
        samples = data_samples['samples'][0] 

        # Save results in the table
        short_addr = "{0:02x}{1:02x}".format(ord(address[6]), ord(address[7]))
        volts = (float(samples['adc-7']) * (1200.0 / 1024.0)) / 1000.0
        query = ("INSERT INTO house.temperature VALUES(NULL, '{0}', "
                  "{1}, {2}, NULL, NULL)".format(short_addr,
                                                 samples['adc-3'],
                                                 volts))
        print (query)
        run_query(db_conn, query)

    # Catch keyboard interrupt (CTRL-C) keypress
    except KeyboardInterrupt:
        break

# Close the port
ser_port.close()

# Disconnect from the server
try:
    db_conn.disconnect()
except:
    pass
