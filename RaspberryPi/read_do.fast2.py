
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

i=0

while os.path.exists("read%i.tsv" %i):
    i+=1

while True:
    line = conn.read()
    ser.write("R\r") # take one reading
    outFile = open("reads%i.tsv" % i, 'a')
    line = conn.read() 
    outFile.write(line + "\t" + (time.strftime("%m/%d/%Y")) + "\t" + (time.strftime ("%H:%M:%S")))
    outFile.write("\n")
    outFile.close()
    print line + "\t" + (time.strftime("%m/%d/%Y")) + "\t"  + (time.strftime("%H:%M:%S"))


