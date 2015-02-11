
import serial
import sys
import RPi.GPIO as GPIO
import time
import atlas


GPIO.setmode(GPIO.BCM)

PinA = 18
PinB = 23
GPIO.setup(PinA, GPIO.OUT)
GPIO.setup(PinB, GPIO.OUT)

GPIO.output(PinA, True)
GPIO.output(PinB, True)

conn = atlas.atlas()
ser = conn.connect()


ser.write("L1\r") # turn on LED
ser.write("C\r")
time.sleep(1)
for i in range(5):
    line = conn.read()
    print line

