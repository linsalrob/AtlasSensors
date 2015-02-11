
'''
Code to just get the information from the probe

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

''' 
To turn a pin on we use:
    GPIO.output(PinA, True)
To turn a pin off we use:
    GPIO.output(PinA, False)
    '''


atl = atlas.atlas()
ser = atl.connect()


# set all the probes to reading
probes = {"Conductivity" : [False, False, 0], "ORP" : [False, True, 1], "DO" : [True, False, 2], "pH" : [True, True, 3]}

# set all the probes to reading
for p in probes:
    print("Turning on " + p + " PinA:" + str(probes[p][0]) + " PinB:" + str(probes[p][1]))
    GPIO.output(PinA, probes[p][0])
    GPIO.output(PinB, probes[p][1])


    ser.write("I\r")
    info = atl.read(10)
    while "," not in info:
        sys.stderr.write("Not : " + info + "\n") 
        info = atl.read(10)
    
    print("Information from probe ["+ str(probes[p][0]) + ":" + str(probes[p][1]) + "]" + str(info))

    print("")

GPIO.cleanup()


