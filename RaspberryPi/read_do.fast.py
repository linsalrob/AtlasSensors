
import serial
import sys
import RPi.GPIO as GPIO
import time
import atlas
import os

GPIO.setmode(GPIO.BCM)


conn = atlas.atlas()
ser = conn.connect()


ser.write("L1\r") # turn on LED

while True:
    line = conn.read()
    print line + "\t" + (time.strftime("%m/%d/%Y")) + "\t"  + (time.strftime("%H:%M:%S"))
    ser.write("R\r") # take one reading
