
/********************************************************
 * This is our header information from other code
 * 
 * Current Device IDs:
 * 0x68  RTC Adafruit DS1307 https://www.adafruit.com/products/264
 * 	
 * 	0x61 DO sensor http://atlas-scientific.com/product_pages/circuits/ezo_do.html?
 * 	0x62 ORP sensor http://atlas-scientific.com/product_pages/circuits/ezo_orp.html?
 * 	0x63 pH sensor http://atlas-scientific.com/product_pages/circuits/ezo_ph.html
 * 	0x64 Conductivity (EC) Sensor http://atlas-scientific.com/product_pages/circuits/ezo_ec.html?
 * 
 * The EC probe measures Electrical Conductivity (EC), Total Dissolved Solids (TDS), Salinity and Specific Gravity (SG) of sea water
 * 
 ********************************************************/

/************************
 * Libraries that we need and global variables
 * 
 ***************************/

#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

RTC_DS1307 rtc;

// real time clock
const int rtc_address = 0x68;
// the atlas addresses

int doProbe = 0x61;
int orpProbe = 0x62;
int phProbe = 0x63;
int ecProbe = 0x64;

// variable of the file we will write to
File outputFile;


void setup() {
  Serial.begin(9600);
  //note that the micro serial connection is not quite the same as the uno
  //see http://arduino.cc/en/Guide/ArduinoLeonardoMicro
  while (!Serial);
  Wire.begin();
  // set the real time clock
  rtc.adjust(DateTime(__DATE__, __TIME__));
  rtc.begin();
  if (!SD.begin(SS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  outputFile = SD.open("calibration.txt", FILE_WRITE);

 
  Serial.println("###################################################################");
  Serial.println("#                                                                 #");
  Serial.println("#                                                                 #");
  Serial.println("#                  Atlas Probe Calibration                        #");
  Serial.println("#                                                                 #");
  Serial.println("#                      version 1.0                                #");
  Serial.println("#                                                                 #");
  Serial.println("#              By Rob Edwards and Jeff Sadural                    #");
  Serial.println("#                                                                 #");
  Serial.println("#                           2015                                  #");
  Serial.println("#                                                                 #");
  Serial.println("###################################################################");
  Serial.println("\n\n");
  Serial.println("We have set the clock, now we need to calibrate the probes");
  Serial.println("To begin, please do the following");
  Serial.println("               1. Dip the DO probe (yellow) in H2O, leave for two minutes, and then take out and leave in the air.");
  Serial.println("               2. Put the pH probe (red) in pH 7 solution. (You will also need pH4 and pH10 solutions");
  Serial.println("               3. Place the ORP probe (blue) in calibration solution. We use 225mV solution. If you don't have that contact Rob or Jeff");
  Serial.println("               4. Make sure the Conductivity probe (Green) is dry");
  Serial.println("\n\n");
  Serial.println("Press return when you are ready");

  String input = keyboard_input();

  /****************************************************************************
   *                      pH calibration                                      *
   ****************************************************************************/

  Serial.println("\n\n\n\nGreat. We will start with the pH probe");
  Serial.println("It takes about 4 seconds for the probe to adjust to each solution and be calibrated");
  Serial.println("If the probe is in pH 7 solution, press return");
 
  input = keyboard_input();
  char mid[] = "cal,mid,7.00\0";
  send_to_probe(phProbe, mid);
  Serial.println("Calibrating the probe .... ");
  delay(2000);
  Serial.println("Move the probe to pH 4 solution and press return");
  input = keyboard_input();
  Serial.println("Calibrating the probe .... ");
  char low[] = "cal,low,4.00\0";
  send_to_probe(phProbe, low);
  delay(2000);
  Serial.println("Move the probe to pH 10 solution and press return");
  input = keyboard_input();
  Serial.println("Calibrating the probe .... ");
  char hi[] = "cal,high,10.00\0";
  send_to_probe(phProbe, hi);
  delay(2000);

  Serial.println("The pH probe is calibrated.\n");
  
   /****************************************************************************
   *                      ORP calibration                                     *
   ****************************************************************************/
  
  Serial.println("Now we will calibrate the ORP probe\n");
  Serial.println("Once the ORP probe (blue) is in the calibration solution press return");
  input = keyboard_input();
  Serial.println("Calibrating the probe .... ");
  char orpc[] = "cal,225\0";
  send_to_probe(phProbe, orpc);
  delay(2000);

  Serial.println("The ORP probe is calibrated.\n");
  
   /****************************************************************************
   *                      EC calibration                                       *
   ****************************************************************************/
 
  Serial.println("Now we will calibrate the EC probe (green)\n");
  Serial.println("If the probe is DRY press return\n");
  input = keyboard_input();
  Serial.println("Calibrating the probe .... ");
  char ecdry[] = "cal,dry\0";
  delay(2000);
  Serial.println("We need to get some calibration solutions here!! HELP");
  
  
  /****************************************************************************
   *                      DO calibration                                      *
   ****************************************************************************/
  
  // Note that we do this last so that the probe has sat around in air while they
  // are doing the other calibrations
  Serial.println("Now lets calibrate the DO probe");
  
  String date = rtc_time();

  outputfile.println("Calibrated all probes and set the clock at " +  date);
  outputfile.flush();

  Serial.println("All the probes are calibrated and the clock is set. Good luck");
  Serial.println("Please note, that most of these calibrations would be more accurate if we included the temperature of the solutions. We don't have that ... yet!");
  
}
void loop  () {
  // we don't do anything here!
}



String rtc_time() {
  DateTime now = rtc.now();

  String date = String(now.month(), DEC) + "/" + String(now.day(), DEC) + "/" + String(now.year(), DEC) + "\t";
  date +=  String(now.hour(), DEC) + ":" + String(now.minute()) + ":" + String(now.second());
  return date;
}



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




void send_to_probe(int probe_address, char message[]) {
  Wire.beginTransmission(probe_address); //call the circuit by its ID number.  
  Wire.write(message);        
  Wire.endTransmission();
}

