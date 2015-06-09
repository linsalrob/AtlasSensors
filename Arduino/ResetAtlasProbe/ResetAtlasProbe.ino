#include <Wire.h>
const int probe_address = 0x63;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Resetting pH probe"));
  char reset_p[] = {'X', '\0'};
  Wire.beginTransmission(probe_address); //call the circuit by its ID number.  
  Wire.write(reset_p);        //transmit the command that was sent through the serial port.
  Wire.endTransmission();
  Serial.println("Complete");
}

void loop() {
}
