
import serial
import sys
import RPi.GPIO as GPIO
import time
import atlas
import os

GPIO.setmode(GPIO.BCM)



outputdir = '/home/pi/ProbeReadings'
ts = time.strftime('%Y%m%d-%H:%M:%S', time.localtime()) # time stamp for this file
outputfile = os.path.join(outputdir, "do-" + ts + ".tsv")
sys.stderr.write("Writing DO readings to " + outputfile + "\n")


while True:
    conn = atlas.atlas()
    ser = conn.connect()
    ser.write("L1\r") # turn on LED
    ser.write("R\r") # take one reading
    for i in xrange(10):
        line = conn.read()
    with open(outputfile, 'a')  as outFile:
        outFile.write(line + "\t" + (time.strftime("%m/%d/%Y")) + "\t" + (time.strftime("%H:%M:%S")) + "\n")
    print line + "\t" + (time.strftime("%m/%d/%Y")) + "\t"  + (time.strftime("%H:%M:%S"))
    ser.write("L0\r") # turn off LED
    time.sleep(10)
    ser.close()
