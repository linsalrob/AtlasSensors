

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

// Define our addresses here (if necessary)
const int rtc_address = 0x68;
// the atlas addresses
int atlas[] = {0x61, 0x62, 0x63, 0x64};
int num_probes = 4;

// our SS pin on the micro is at output 17
//byte SS = 17;
File outputFile;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  if (!SD.begin(SS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  outputFile = SD.open("output.txt", FILE_WRITE);
  Serial.flush();
  Serial.println(F("\n\nDate\tTime\tDO\tORP\tpH\tEC\tTDS\tSAL\tSG"));
}


void loop() {
  // initiate all the probes that we are checking

  for (int i=0; i < num_probes; i++) {
    initiate_atlas(atlas[i]);
  }
  unsigned long time = millis();
 String date = rtc_time(); 
 Serial.print(date);
 outputFile.print(date);

  while ((millis() - time) < 1500) {
    delay(1501-(millis() - time));
  }
  
  for (int i=0; i < num_probes; i++) {
    String res = read_atlas(atlas[i]);
    Serial.print("\t" + res);
    outputFile.print("\t" + res);
  }
  Wire.flush();

  outputFile.println();
  outputFile.flush();
  Serial.println();
  delay(5000);
}



String rtc_time() {
  DateTime now = rtc.now();
    
  String date = String(now.month(), DEC) + "/" + String(now.day(), DEC) + "/" + String(now.year(), DEC) + "\t";
  date +=  String(now.hour(), DEC) + ":" + String(now.minute()) + ":" + String(now.second());
  return date;
}


/*

*********************************************************
*                                                       *
*               Real Time Clock                         *
*                                                       *
*********************************************************

For the real time clock we are going to use the code available from gravitech http://site.gravitech.us/MicroResearch/I2C/I2C-RTC/I2C-RTC-Arduino.pde
This just displays the time. At some point we need to have set the time!

*/



String RTC_time() {
  char tempchar [7];
  byte i = 0;
  Wire.beginTransmission(rtc_address);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(rtc_address, 7);
  while(Wire.available()) {
    tempchar[i] = Wire.read(); // receive a byte as character 
    i++;
  } 
  byte Second = tempchar[0];
  byte Minute = tempchar[1];
  byte Hour   = tempchar[2];
  byte Day    = tempchar[3];
  byte Date   = tempchar[4];
  byte Month  = tempchar[5];
  byte Year   = tempchar[6];

  // Display time
  String ret = "";
  ret += String(Month, HEX) + "/" + String(Date, HEX) + "/";
  if (Year<10)
    ret += "0";
  ret += String(Year, HEX) + "\t";
  if (Hour < 10) 
    ret += "0";
  ret += String(Hour, HEX) + ":";
  if (Minute < 10) 
    ret += "0";
  ret += String(Minute, HEX) + ":";
  if (Second < 10)
    ret += "0";
  ret += String(Second, HEX);
  
  return ret;

}


/*

*********************************************************
*                                                       *
*                 Atlas Probes                          *
*                                                       *
*********************************************************

This code is based on code from Atlas Scientific at https://www.atlas-scientific.com/_files/code/ph-i2c.pdf?

Note that all of the I2C code for the Atlas Probes is the same ... nice! Therefore, we just need this code and
the I2C address that we are sending the data to.

We do this in two steps, because the Atlas Code needs to first tell the probe to read, and then get the answer back.
There can be upto 320 millisecond delay between initiation and reading, but we wait for one second (in the loop()) to 
be sure that the reading has been made.

*/

void initiate_atlas(int probe_address) {
  // Tell the probe to take a reading. 
  char read_probe[] = {'r', '\0'};
  Wire.beginTransmission(probe_address); //call the circuit by its ID number.  
  Wire.write(read_probe);        
  Wire.endTransmission();
}
  
  
  
String read_atlas(int probe_address) {
  // read the pH. Hopefully initiate ph has been called before ... should we set a boolean to test that?
  
  byte code=0;                     //used to hold the I2C response code. 
  char data[60];                   //we make a 60 byte character array to hold incoming data from the probe. This is much more than we need, but the EC probe needs 48 bytes 
  byte in_char=0;                  //used as a 1 byte buffer to store in bound bytes from the probe.   
  byte i=0;                        //counter used for ph_data array. 

  Wire.requestFrom(probe_address,60,1); //call the circuit and request 60 bytes (this may be more then we need).
  code=Wire.read();
  if (code != 1)
    return "There was an error reading the pH. Code: " + String(code);
  
   while(Wire.available()){          //are there bytes to receive.   
    in_char = Wire.read();           //receive a byte.
    data[i]= in_char;             //load this byte into our array.
    i+=1;                            //incur the counter for the array element. 
    if(in_char==0){                 //if we see that we have been sent a null command. 
      i=0;                        //reset the counter i to 0.
      Wire.endTransmission();     //end the I2C data transmission.
      break;                      //exit the while loop.
    }
  }
  Wire.flush();
  
  String response = String(data);
  // need to convert , in the ec response to \t
  if (probe_address == 0x64)
    response.replace(",", "\t");
  return String(response);  
}



