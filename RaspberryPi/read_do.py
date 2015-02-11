
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
#ser.write("%\r") # enable percent saturation in the output
time.sleep(1)
i = 0
while os.path.exists("reads%i.tsv" %i):
    i+=1

while True:
    #outFile = open("reads%i.tsv" % i, 'a')
    line = conn.read()
    #outFile.write(line + "\t" + (time.strftime("%m/%d/%Y")) + "\t" + (time.strftime("%H:%M:%S")))
    #outFile.write("\n")
    #outFile.close()
    print line + "\t" + (time.strftime("%m/%d/%Y")) + "\t"  + (time.strftime("%H:%M:%S"))
    ser.write("L0\r") # turn off LED
    time.sleep(5)
    ser.write("L1\r") # turn on LED
    ser.write("R\r") # take one reading
    time.sleep(1) # the reading takes 650 ms to complete
