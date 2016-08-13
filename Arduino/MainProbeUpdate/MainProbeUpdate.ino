/********************************************************
Read the devices on the I2C bus
Current Device IDs:
        0x68  RTC Adafruit DS1307 https://www.adafruit.com/products/264

	0x61 DO sensor http://atlas-scientific.com/product_pages/circuits/ezo_do.html?
	0x62 ORP sensor http://atlas-scientific.com/product_pages/circuits/ezo_orp.html?
	0x63 pH sensor http://atlas-scientific.com/product_pages/circuits/ezo_ph.html
	0x64 Conductivity (EC) Sensor http://atlas-scientific.com/product_pages/circuits/ezo_ec.html?
The EC probe measures Electrical Conductivity (EC), Total Dissolved Solids (TDS), Salinity and Specific Gravity (SG) of sea water
********************************************************/

#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>


RTC_DS1307 rtc;
/**********************************************
 * Address declrations for circuits.
 * Cirucits communicate through the I2C protocol,
 * so all circuits need addresses so that the
 * Arduino can send commands or receive data.
 *
 * The default addresses are used for the
 * Arduino circuits.
 **********************************************/

// RTC - Real Time Clock
const int rtc_address = 0x68;

/**************************************************
 * Since the circuit addresses don't change, we can
 * hold them in an array
 **************************************************/
int atlas[] = {0x61, 0x62, 0x63, 0x64};
const int num_probes = 4;

// Creates the file to hold the readings
File outputFile;

void setup() {

  // Starts the serial port on the Arduino,
  // transmiting at 9600 baud
  Serial.begin(9600);

  // Starts the I2C communication protocol
  Wire.begin();

  // Initialize our real time clock, must be
  // called before any other commands on the clock
  // are called
  rtc.begin();

  // Initialze and check the microSD card reader.
  // If the card reader fails, message is displayed.
  if (!SD.begin(SS)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Open the file for writing the data
  outputFile = SD.open("output.txt", FILE_WRITE);

  // Waits for transmission of any outgoing data to complete
  Serial.flush();

  // Writes data to both serial monitor and microSD card
  Serial.println(F("\n\nDate\tTime\tDO\tORP\tpH\tEC\tTDS\tSAL\tSG"));
  outputFile.println(F("\n\nDate\tTime\tDO\tORP\tpH\tEC\tTDS\tSAL\tSG"));

  // initiate all the probes for reading
  for (int i = 0; i < num_probes; i++) {
    initiate_atlas(atlas[i]);
  }


}


/**************************************************
 * Main loop that runs continuously
 **************************************************/
void loop() {

  // Get current time and date
  // for readings, and write to
  // serial port and card.
  unsigned long time = millis();
  String date = rtc_time();
  Serial.print(date);
  outputFile.print(date);

  // Delay and reset to prevent clock
  // overflow.
  while ((millis() - time) < 1500) {
    delay(1501 - (millis() - time));
  }

  // Write probe readings to 
  // card and serial monitor.
  // Reads in this order:
  // DO, ORP, pH, EC
  for (int i = 0; i < num_probes; i++) {
    String res = read_atlas(atlas[i]);
    Serial.print("\t" + res);
    outputFile.print("\t" + res);
  }

  // Clears buffer once all data is sent
  Wire.flush();

  // Write to memory card and serial monitor
  outputFile.println();
  outputFile.flush();
  Serial.println();

  // Time is in miliseconds
  delay(10000);
}
/**************************************************
 * End Main Loop
 **************************************************/

 
/**************************************************
 * RTC
 **************************************************/
String rtc_time() {
  DateTime now = rtc.now();

  String date = String(now.month(), DEC) + "/" + String(now.day(), DEC) + "/" + String(now.year(), DEC) + "\t";
  if (now.hour() < 10)
    date += "0";
  date +=  String(now.hour(), DEC) + ":";
  if (now.minute() < 10)
    date += "0";
  date += String(now.minute(), DEC) + ":";
  if (now.second() < 10)
    date += "0";
  date += String(now.second(), DEC);
  return date;
}



/*
*********************************************************
*                                                       *
*                 Atlas Probes                          *
*                                                       *
*********************************************************
This code is based on code from Atlas Scientific at https://www.atlas-scientific.com/_files/code/ph-i2c.pdf?
We do this in two steps, because the Atlas Code needs to first tell the probe to read, and then get the answer back.
There can be upto 320 millisecond delay between initiation and reading, but we wait for one second (in the loop()) to
be sure that the reading has been made.
*/

void initiate_atlas(int probe_address) {
  // Tell the probe to take a reading.
  char read_probe[] = {'r', '\0'};
  Wire.beginTransmission(probe_address);
  Wire.write(read_probe);
  Wire.endTransmission();
}

String read_atlas(int probe_address) {
  byte code = 0;                  
  char data[60];                   
  byte in_char = 0;                
  byte i = 0;            

  Wire.requestFrom(probe_address, 60, 1);
  code = Wire.read();
  if (code != 1)
    return "There was an error reading the pH. Code: " + String(code);

  while (Wire.available()) {        //are there bytes to receive.
    in_char = Wire.read();           //receive a byte.
    data[i] = in_char;            //load this byte into our array.
    i += 1;                          //incur the counter for the array element.
    if (in_char == 0) {             //if we see that we have been sent a null command.
      i = 0;                      //reset the counter i to 0.
      Wire.endTransmission();     //end the I2C data transmission.
      break;                      //exit the while loop.
    }
  }
  Wire.flush();

  String response = String(data);
  if (probe_address == 0x64)
    response.replace(",", "\t");
  return String(response);
}

