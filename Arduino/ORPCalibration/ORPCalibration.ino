#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

int orpProbe = 0x62;

void setup() {
  Serial.println("Place the ORP probe in the ORP solution and press return");
  keyboard_input();
  send_to_probe(orpProbe, "Cal,clear\0");
  
  char orpc[] = "Cal,225\0";
  delay(120000);
  send_to_probe(orpProbe, orpc);

  Serial.println("The ORP probe is calibrated.\n");
}

void loop() {
  // put your main code here, to run repeatedly:

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
