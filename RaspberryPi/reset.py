import serial


# connect to the board via the TX/RX lines
tty = '/dev/ttyAMA0'
ser = serial.Serial(tty, 38400)

import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)

PinA = 18
PinB = 23
GPIO.setup(PinA, GPIO.OUT)
GPIO.setup(PinB, GPIO.OUT)

GPIO.output(PinA, False)
GPIO.output(PinB, False)

ser.write("X\r")

GPIO.cleanup()
