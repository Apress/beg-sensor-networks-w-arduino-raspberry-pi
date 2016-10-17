# RasPi Simple Sensor - Beginning Sensor Networks
# 
# For this script, we explore a simple sensor (a pushbutton) and a simple
# response to sensor input (a LED). When the sensor is activated (the 
# button is pushed), the LED is illuminated.

import RPi.GPIO as GPIO   # GPIO library

# Pin assignments
LED_PIN = 7
BUTTON_PIN = 17

# Setup GPIO module and pins
GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)
GPIO.setup(BUTTON_PIN, GPIO.IN)

# Set LED pin to OFF (no voltage) 
GPIO.output(LED_PIN, GPIO.LOW)

# Loop forever
while 1:
    # Detect voltage on button pin
    if GPIO.input(BUTTON_PIN) == 1:
        # Turn on the LED
        GPIO.output(LED_PIN, GPIO.HIGH)
    else:
        # Turn off the LED
        GPIO.output(LED_PIN, GPIO.LOW)

