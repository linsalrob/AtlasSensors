
#include <Wire.h>

/*
	
	0x61 (97) DO sensor http://atlas-scientific.com/product_pages/circuits/ezo_do.html?
	0x62 (98) ORP sensor http://atlas-scientific.com/product_pages/circuits/ezo_orp.html?
	0x63 (99) pH sensor http://atlas-scientific.com/product_pages/circuits/ezo_ph.html
	0x64 (100) Conductivity Sensor http://atlas-scientific.com/product_pages/circuits/ezo_ec.html?
*/

const int ph_address = 0x61;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.flush();
  Serial.println(F("Reading"));
}

void loop() {
   initiate_ph();
   delay(3000);
   String t = read_ph();
   Serial.println(t);
   delay(3000);
}


void initiate_ph() {
  // Tell the pH probe to take a reading. We do this before we probe the other sensors, 
  // and then the reading should be ready when we need it.
  char readpH[] = {0x72, 0x00};
  Serial.println("Initiating : >>" + String(readpH) + "<<");
  Wire.beginTransmission(ph_address); //call the circuit by its ID number.  
  Wire.write(readpH);        //transmit the command that was sent through the serial port.
  byte error = Wire.endTransmission();  
  if (error != 0)
   Serial.println("There was an error initiating the probe"); 
}
  
  
  
String read_ph() {
  // read the pH. Hopefully initiate ph has been called before ... should we set a boolean to test that?
  
  byte code=0;                     //used to hold the I2C response code. 
  char ph_data[20];                //we make a 20 byte character array to hold incoming data from the pH 
  byte in_char=0;                  //used as a 1 byte buffer to store in bound bytes from the pH Circuit.   
  byte i=0;                        //counter used for ph_data array. 

  Wire.requestFrom(ph_address,20,1); //call the circuit and request 20 bytes (this may be more then we need).
  code=Wire.read();
  if (code != 1)
    return "There was an error reading the pH. Code: " + String(code);
  
  byte error;
  while(Wire.available()){          //are there bytes to receive.   
    in_char = Wire.read();           //receive a byte.
    ph_data[i]= in_char;             //load this byte into our array.
    i+=1;                            //incur the counter for the array element. 
    if(in_char==0){                 //if we see that we have been sent a null command. 
      i=0;                        //reset the counter i to 0.
      error = Wire.endTransmission();     //end the I2C data transmission.
      break;                      //exit the while loop.
    }
  }
  Wire.flush();
  
  if (error != 0)
    Serial.println("There was an error ending the transmission");
  
  return String(ph_data);
  
  
}
