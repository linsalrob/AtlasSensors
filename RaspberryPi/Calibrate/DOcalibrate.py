import serial
import sys
import RPi.GPIO as GPIO

usbport = '/dev/ttyAMA0'
ser = serial.Serial(usbport, 38400)


GPIO.setmode(GPIO.BCM)

# PinA = 18
# PinB = 23
# GPIO.setup(PinA, GPIO.OUT)
# GPIO.setup(PinB, GPIO.OUT)

# GPIO.output(PinA, True)
# GPIO.output(PinB, False)


# turn on the LEDs
ser.write("L1\r")

line = ""
results = ['a', 'b', 'c']

def check(arr):
    d={x:1 for x in arr}
    if len(d.keys()) == 1:
        return True
    return False

exit=False

while True:
  data = ser.read()
  if(data == "\r"):
    if exit:
        print line
        sys.exit()
    print "Received from sensor:" + line
    results.append(line)
    while len(results) > 5:
        results.pop(0)
    if check(results):
        ser.write("M\r")
        print "Your Dissolved Oxygen Sensor is now Calibrated with a reading of " + str(results[0])
        exit=True
        print "The partial pressure in mV read by the probe is : ",

    line = ""
  else:
    line = line + data

ser.write("L0\r")

