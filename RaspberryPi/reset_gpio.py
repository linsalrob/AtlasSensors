#!/usr/bin/python

import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)

PinA = 18
PinB = 23
GPIO.setup(PinA, GPIO.OUT)
GPIO.setup(PinB, GPIO.OUT)

GPIO.output(PinA, False)
GPIO.output(PinB, False)
GPIO.cleanup()

