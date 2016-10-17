#
# Sensor Networks Example Raspberry Pi Xively Cloud Storage
#  
# This project demonstrates how to save sensor data to an Xively feed.
# Requires the use of a A/D converter. Specifically, the ADC1015 from
# Adafruit (http://www.adafruit.com/products/1083).
#
import datetime
import sys
import time
import xively

from Adafruit_ADS1x15 import ADS1x15

XIVELY_API_KEY = "<YOUR_KEY_HERE>"
XIVELY_FEED_ID = <YOUR_FEED_HERE>

ADS1015 = 0x00  # 12-bit A/D Converter

# Set the ADC using the default mode (use default I2C address)
adc = ADS1x15(ic=ADS1015)

def get_temperature():
    # Read channel 0 in single-ended mode, +/- 5V, 500 sps
    volts = adc.readADCSingleEnded(0, 5000, 500)

    tempc = (((float(volts)*1200.0)/1024.0)-500.0)/10.0
    tempf = ((tempc *9.0)/5.0)+32.0

    print "Samples generated: celsius: %f" % tempc,
    print "fahrenheit: %f" % tempf

    return (tempc, tempf)


def record_samples(tempc, tempf):
    api = xively.XivelyAPIClient(XIVELY_API_KEY)
    feed = api.feeds.get(XIVELY_FEED_ID)
    now = datetime.datetime.now()
    print "Uploading to Xively ...",
    feed.datastreams = [
        xively.Datastream(id='celsius', current_value=tempc, at=now),
        xively.Datastream(id='fahrenheit', current_value=tempf, at=now),
    ]
    feed.update()
    print "done."
    time.sleep(5)


def main():
    print "Starting cloud data upload."
    while True:
        tempc, tempf = get_temperature()
        record_samples(tempc, tempf)


if __name__ == '__main__':
    try:
        args = sys.argv[1:]
        main(*args)
    except KeyboardInterrupt:
        pass
