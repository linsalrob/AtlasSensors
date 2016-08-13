#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

int ecProbe = 0x64;

void setup() {
   /****************************************************************************
    *                      EC calibration                                      *
    ****************************************************************************/
  char reset[] = "Cal,clear\0";
  char ecdry[] = "Cal,dry\0";
  char hi[] = "Cal,high,150000\0";
  char low[] = "Cal,low,12880\0";
   
  // EC probe MUST START DRY!
  Serial.println(F("Calibrating the EC probe, must start dry, hit enter when ready... "));
  keyboard_input(); 
  
  send_to_probe(ecProbe, ecdry);
  delay(120000);
  Serial.println("Insert low and hit enter... ");
  keyboard_input(); 
  send_to_probe(ecProbe, low);
  delay(120000);
  Serial.println("Insert high and hit enter... ");
  keyboard_input(); 
  send_to_probe(ecProbe, hi);
  Serial.println(F("EC calibration finished"));
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
