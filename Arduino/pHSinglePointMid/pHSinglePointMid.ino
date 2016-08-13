#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

int phProbe = 0x63;

void setup() {
  Serial.println("Place the pH probe in the pH 10 solution and press return");
  keyboard_input();
  delay(120000);

  // Calibrate the highpoint
  mid_cal();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void mid_cal(){
  char mid[] = "cal,mid,7.00\0";
  send_to_probe(phProbe, mid);
  Serial.println(F("Calibrating the probe .... "));
}

void send_to_probe(int probe_address, char message[]) {
  Wire.beginTransmission(probe_address); //call the circuit by its ID number.  
  Wire.write(message);        
  Wire.endTransmission();
}

// Format keyboard input to ignore unnecessary 
// inputs.
String keyboard_input() {
  char received;
  String inputData;
  while (received != '\n') {      // When new line character is received (\n = LF, \r = CR)
    if (Serial.available() > 0)   // When character in serial buffer read it
    {
      received = Serial.read();  
      Serial.print(received);     // Echo each received character, terminal dont need local echo
      inputData += received;         // Add received character to inData buffer
    }
  }
  inputData.trim();                  // Eliminate \n, \r, blank and other not "printable
  return inputData;
}
