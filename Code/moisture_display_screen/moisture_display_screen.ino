// SparkFun Soil Moisture Sensor and Serial LCD example 1
// By Sarah Al-Mutlaq 2015
// Sketch reads sensor and desplays level and whether soil is wet or dry

// Use the softwareserial library to create a new "soft" serial port
// for the display. This prevents display corruption when uploading code.
#include <Wire.h>

#include "rgb_lcd.h"


#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

rgb_lcd lcd;

// Listen on default port 5555, the webserver on the Yún
// will forward there all the HTTP requests for us.
BridgeServer server;
String startString;
long hits = 0;


// Here we are setting up some water thersholds that we will 
// use later. Note that you will need to change these to match
// your soil type and environment. 

int thresholdUp = 700;
int thresholdDown = 500;

// We are setting up the pin A0 on the Gorve Shield to be our moisture sensor
// pin input:
int sensorPin = A0;


void setup(){
    lcd.begin(16, 2);
    // initialize the serial communications for debugging
    Serial.begin(9600);
    Bridge.begin();
}

void loop(){
  // Here we are declaring a string, which are lines of words,
  // and we want DisplayWords to be the words displayed on
  // the LCD screen, which will change based on whether the soil
  // wet or dry based on our threshold values above.
  String DisplayWords;

  // We need to set up a pin to get the value that the soil 
  // moisture sensor is outputting, so sensorValue will get the
  // analog value from the sensor pin A0 on the redboard that we 
  // set up earlier.

  int sensorValue;
  sensorValue = analogRead(sensorPin);

  // Clear both lines of the LCD
  lcd.setCursor(0, 0);
  lcd.write("                "); 
  lcd.setCursor(0, 1);
  lcd.write("                ");
  
  //Write what we want to desplay on the first line of the LCD:
  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(sensorValue);

  //Write what we want to display on the serial connection for debugging:
  
  Serial.write("Water Level: ");
  Serial.print(sensorValue); //Using .print instead of .write for values
  
  // move cursor to beginning of second line on LCD:
  lcd.setCursor(0, 1);


  if (sensorValue <= thresholdDown){
    // Now we are going to check if the water level is below a 
    // out thresholdDown value we set earlier, and if it is have 
    // words "Dry, Water it!" display one column over on the second 
    // row:
    DisplayWords = "Dry, Water it!";

  } else if (sensorValue >= thresholdUp){
    // If the value is not below our thresholdDown value we want to 
    // check if it is above our thresholdUp value, and if it is 
    // change the display words to "Wet, Leave it!":

    DisplayWords = "Wet, Leave it !";

  } else {
    // Otherwise say nothing
    DisplayWords = "";
  }

  Serial.print(DisplayWords);
  lcd.print(DisplayWords);

  delay(500); //wait for half a second, so it is easier to read
}
