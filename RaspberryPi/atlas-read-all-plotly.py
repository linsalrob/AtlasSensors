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
import plotly.plotly as py
from plotly.graph_objs import Scatter, Stream
import argparse


parser = argparse.ArgumentParser(description='Read the probes and write the data to a file. Send a stream to plotly')
parser.add_argument('-f', '--file', help='The file name to write to (required)', required=True)
parser.add_argument('-p', '--plot', help='The stream to plot (default = DO)', default='DO', choices=['DO', 'pH', 'Conductivity', 'Salinity'])
args = parser.parse_args()



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
names=["Conductivity", "ORP", "DO", "pH"]

## set up plotly
# py.sign_in(username, api_key)
stream_ids = py.get_credentials()['stream_ids'][5:9]

py.iplot([Scatter(x=[], y=[], name=names[k], stream=Stream(token=stream_ids[k])) for k in range(len(stream_ids))], filename = 'Probes', fileopt='extend')

streams = [py.Stream(stream_id) for stream_id in stream_ids]
for stream in streams:
    stream.open()

# set all the probes to reading
with open(args.file, "w") as writer:
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

        writer.write("# Probe: " + p + " ["+ str(probes[p][0]) + ":" + str(probes[p][1]) + "]" + str(info) + "\n")
        writer.flush()

    try:
        while True:
            #now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
            now = datetime.datetime.now()
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

                try:
                    line = float(line)
                except:
                    print "Couldn't parse float: ", line
                    continue

                writer.write("\t".join([p, str(now), str(line)]) + "\n")
                # sys.stderr.write("writing to " + str(probes[p][2]) + " which is " + str(stream_ids[probes[p][2]]) + "\n")
                streams[int(probes[p][2])].write({'x': now, 'y': line})

            writer.flush()

    except Exception as err:
        print "Unexpected error:", sys.exc_info()[0], " :: ", err
        GPIO.cleanup()








