'''
Code to iteratively read all the probes using the Multi Circuit UART Carrier Board from Atlas.

The board uses the following combination:
    A   B       Probe   Type
    0   0       1       Conductivity
    0   1       2       ORP
    1   0       3       DO
    1   1       4       pH


Wire the MCUCB with;
    VCC connected to 5V
    GND connected to GND
    TX  connected to RX
    RX  connected to TX
    E   connected to GND
    A   connected to 18
    B   connected to 23

Make sure that you have disabled the serial tty in /etc/inittab

'''

import sys
import datetime
import RPi.GPIO as GPIO
import time
import serial
import atlas

GPIO.setmode(GPIO.BCM)

PinA = 18
PinB = 23
GPIO.setup(PinA, GPIO.OUT)
GPIO.setup(PinB, GPIO.OUT)


conn = atlas.atlas()
ser = conn.connect()

# set all the probes to reading
#probes = {"Conductivity" : [False, False, 0], "ORP" : [False, True, 1], "DO" : [True, False, 2], "pH" : [True, True, 3]}
probes = {"ORP" : [False, True, 1], "DO" : [True, False, 2], "pH" : [True, True, 3]}

# set all the probes to reading
for p in probes:
    GPIO.output(PinA, probes[p][0])
    GPIO.output(PinB, probes[p][1])
    time.sleep(1)
    ser.write("L0\r") # turn off LED
    time.sleep(0.1)
    ser.write("L0\r") # turn off LED

    
    ser.write("I\r")
    info = conn.read()
    while "," not in info:
        info = conn.read()

    print("Information from probe ["+ str(probes[p][0]) + ":" + str(probes[p][1]) + "]" + str(info))

try:
    while True:
        now = datetime.datetime.now()
        output=[0, 0, 0, 0]
        for p in probes:
            GPIO.output(PinA, probes[p][0])
            GPIO.output(PinB, probes[p][1])
            time.sleep(0.5)
            ser.write("L1\r") # turn on LED
            ser.write("C\r")
            time.sleep(1)
            for i in range(5):
                line = conn.read()

            ser.write("L0\r") # turn off LED
            ser.write("E\r")
            if '*ER' in line:
                sys.stderr.write("Probe " + p + " is not attached\n")
                continue

            if '*OK' in line:
                sys.stderr.write("Probe " + p + " is OK, but not ready\n")
                continue

            print("\t".join([p, str(now), str(line)]) + "\r")

            ## py.plot([now], output, filename='Probes', fileopt='extend', traces=lines)
except Exception as err:
    print "Unexpected error:", sys.exc_info()[0], " :: ", err
    GPIO.cleanup()








