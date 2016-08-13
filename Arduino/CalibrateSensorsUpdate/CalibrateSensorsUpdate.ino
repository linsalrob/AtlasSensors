/*************************************************
 * We only run the setup function in this program.
 * The main program to run the probes is the 
 * MainProbeSoftware.ino
 *************************************************/

/*********************************************
 * Libraries and global variables
 *********************************************/

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
// RTC (real time clock)
const int rtc_address = 0x68;

// Atlas Sensors
int doProbe = 0x61;
int orpProbe = 0x62;
int phProbe = 0x63;
int ecProbe = 0x64;

File outputFile;

void setup() {
  Serial.begin(9600);
  
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
  
 /*#################################################################
  #                                                                 #
  #                                                                 #
  #                  Atlas Probe Calibration                        #
  #                                                                 #
  #                                                                 #
  #              By Rob Edwards and Jeff Sadural                    #
  #                                                                 #
  #                           2016                                  #
  #                                                                 #
  ###################################################################
  
  We have set the clock, now we need to calibrate the probes
  To begin, please do the following;
                 1. Dip the DO probe in H2O, leave for two minutes, and then take out and leave in the air.
                 2. Put the pH probe in pH 7 solution (You will also need pH4 and pH10 solutions).
                 3. Place the ORP probe in calibration solution.
                 4. Make sure the Electric Conductivity probe is dry.*/

  // Begin serial monitor on screen instructions
  Serial.println(F("Welcome! Let's calibrate our probes!"));
  Serial.println(F("Each pH calibration will take 2 minutes"));
  Serial.println(F("Rinse the pH probe and place in pH 7 solution and hit return..."));
  String input = keyboard_input();

  /****************************************************************************
   *                      pH calibration                                      
   * Only the midpoint calibration is required. We use 3 point calibration                     
   * for improved accuracy. Midpoint calibration is required, low and high can
   * be done at anytime after the midpoint calibration. We calibrate all three
   * points in this program.
   ****************************************************************************/

  // We will start with the pH probe
  // It takes about 2 minutes for the probe to adjust to each solution and be calibrated
  // Once the probe is in pH 7 solution, press return

  // Clear any previous calibration data
  send_to_probe(phProbe, "Cal,clear"); 
  keyboard_input();

  //time delay is in milliseconds
  delay(120000);

  // Calibrate the midpoint
  mid_cal();
  
  
  Serial.println(F("Rinse the pH probe and place in to pH 4 solution and press return"));
  keyboard_input();

  // Calibrate the low point
  low_cal();
  
  Serial.println("Move the probe to pH 10 solution and press return");
  keyboard_input();
  delay(120000);

  // Calibrate the highpoint
  high_cal();
  
  Serial.println("The pH probe is calibrated, please press return.\n");
  keyboard_input();
  
   /***************************************************************************
   *                      ORP calibration                                     *
   ****************************************************************************/
  Serial.println(F("Calibrating the ORP probe, hit return when ready..."));
  input = keyboard_input();
  send_to_probe(orpProbe, "Cal,clear\0");
  
  char orpc[] = "Cal,225\0";
  send_to_probe(orpProbe, orpc);
  delay(5000);

  Serial.println("The ORP probe is calibrated.\n");
  
   /****************************************************************************
    *                      EC calibration                                      *
    ****************************************************************************/
 
  // EC probe MUST START DRY!
  Serial.println(F("Calibrating the EC probe, must start dry, hit enter when ready... "));
  input = keyboard_input(); 
  send_to_probe(ecProbe, "Cal,clear\0");
  char ecdry[] = "Cal,dry\0";
  send_to_probe(ecProbe, ecdry);
  delay(5000);
  Serial.println("Insert low and hit enter... ");
  input = keyboard_input(); 
  send_to_probe(ecProbe, "Cal,low,12880\0");
  delay(5000);
  Serial.println("Insert high and hit enter... ");
  input = keyboard_input(); 
  send_to_probe(ecProbe, "Cal,high,150000\0");
  delay(5000);
  
  
  /****************************************************************************
   *                      DO calibration                                      *
   ****************************************************************************/
  
  // Note that we do this last so that the probe has sat around in air while they
  // are doing the other calibrations
  // Only needs to be calibrated once per year
  Serial.println("Now lets calibrate the DO probe");
  send_to_probe(doProbe, "Cal,clear\0");
  char doCal[] = "Cal,1\0";
  send_to_probe(doProbe, doCal);
  delay(5000);
  
  String date = rtc_time();

  outputFile.println("Calibrated all probes and set the clock at " +  date);
  outputFile.flush();

  Serial.println(F("All the probes are calibrated and the clock is set. Good luck"));  
}
void loop  () {
  // Empty as this program only needs to run calibration once.
}


   /***************************************************************************
   *                      Circuit realted functions                           *
   ****************************************************************************/

String rtc_time() {
  DateTime now = rtc.now();

  String date = String(now.month(), DEC) + "/" + String(now.day(), DEC) + "/" + String(now.year(), DEC) + "\t";
  date +=  String(now.hour(), DEC) + ":" + String(now.minute()) + ":" + String(now.second());
  return date;
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

void send_to_probe(int probe_address, char message[]) {
  Wire.beginTransmission(probe_address); //call the circuit by its ID number.  
  Wire.write(message);        
  Wire.endTransmission();
}

   /***************************************************************************
   *                      pH Calibration Functions
   ****************************************************************************/
void mid_cal(){
  char mid[] = "cal,mid,7.00\0";
  send_to_probe(phProbe, mid);
  Serial.println(F("Calibrating the probe .... "));
}

void low_cal(){
  Serial.println("Calibrating the probe .... ");
  char low[] = "cal,low,4.00\0";
  send_to_probe(phProbe, low);
}

void high_cal(){
  Serial.println("Calibrating the probe .... ");
  char hi[] = "cal,high,10.00\0";
  send_to_probe(phProbe, hi);
}

// Function for calibrating with just the pH 7 solution...
void singlePointCal(){
  char resetSingle[] = "cal,clear\0";
  send_to_probe(phProbe, resetSingle);
  delay(1400);
}

// Returns the number of points calibrated
// up to 3: mid, low, high (Probe is calibrated in this order!)
int calStatus(){
  char calStatus[] = "cal,?\0";
  send_to_probe(phProbe, calStatus);
  delay(300);
  Wire.requestFrom(phProbe,20,1);
  Serial.print(Wire.read());
}

