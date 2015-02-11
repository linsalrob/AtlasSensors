
import sys
import time
import serial

class atlas():

    def __init__(self):
        self.ser=""

    def connect(self):
        '''Connect to the atlas probes'''
        # connect to the board via the TX/RX lines
        tty = '/dev/ttyAMA0'
        self.ser = serial.Serial(tty, 38400)
        return self.ser

    def setSerial(self, ser):
        '''Set the serial connection'''
        self.ser = ser

    def read(self, timeout=60):
        '''Read from the serial port. Time out after a predefined amount of time (default 60s)'''

        start = time.time()

        result=""
        data = self.ser.read()
        while data != '\r':
            result += data
            data = self.ser.read()
            if time.time() > start+timeout:
                sys.stderr.write("Probe did not return a <CR>\n")
                break

        return result


