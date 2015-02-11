import RPi.GPIO as GPIO
import serial
import atlas


GPIO.setmode(GPIO.BCM)

PinA = 18
PinB = 23
GPIO.setup(PinA, GPIO.OUT)
GPIO.setup(PinB, GPIO.OUT)


conn = atlas.atlas()
ser = conn.connect()

#probes = {"Conductivity" : [False, False, 0], "ORP" : [False, True, 1], "DO" : [True, False, 2], "pH" : [True, True, 3]}
probes = {"ORP" : [False, True, 1], "DO" : [True, False, 2], "pH" : [True, True, 3]}


for p in probes:
    GPIO.output(PinA, probes[p][0])
    GPIO.output(PinB, probes[p][1])
    ser.write("x\r") # turn off LED
    ser.write("L1\r")

GPIO.cleanup()
