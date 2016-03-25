#include <Wire.h>
#include "rgb_lcd.h"
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains account information

const String DM_TARGET = "YOUR_TWITTER_HANDLE";

rgb_lcd lcd;

// Here we are setting up some water thersholds that we will 
// use later. Note that you will need to change these to match
// your soil type and environment. 

int thresholdUp = 700;
int thresholdDown = 500;

// Record and Configure after how many indication-samples (either high or low) we
// want to a) warn the user throught the display and b) actually send out our DM
int TweetThreshold = 0;
int TweetThresholdWarning = 75;
int TweetThresholdLimit = 100;


// The following variables are part of a Moving Average filter that smooths
// the sensor values over 16 samples.
const int LENBUF = 16;
int sensebuffer[LENBUF];
int senseid = 0;

int senseaverage(){
  int x=0;
  for (int i=0; i<LENBUF; i++){
    x+=sensebuffer[i];
  }
  x/=LENBUF;
  return x;
}

// We are setting up the pin A0 on the Gorve Shield to be our moisture sensor
// pin input:
int sensorPin = A0;


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  Bridge.begin();
  
  

  for (int i=0; i<LENBUF; i++){
    sensebuffer[i]=analogRead(sensorPin);
  }
  
}

void loop() {
  // Here we are declaring a string, which are lines of words,
  // and we want DisplayWords to be the words displayed on
  // the LCD screen, which will change based on whether the soil
  // wet or dry based on our threshold values above.
  String DisplayWords;

  // Same function as DisplayWords but for tweeting
  String TweetWords;

  // We need to set up a pin to get the value that the soil 
  // moisture sensor is outputting, so sensorValue will get the
  // analog value from the sensor pin A0 on the redboard that we 
  // set up earlier.

  int sensorValue;
  sensorValue = analogRead(sensorPin);

  // Add this new value to the sensor ring-buffer and take the
  // average of the past 16 observations
  sensebuffer[senseid] = sensorValue;
  sensorValue = senseaverage();
  senseid=(senseid+1)%LENBUF;

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

  // Now we are going to check if the water level is below a 
  // out thresholdDown value we set earlier, and if it is have 
  // words "Dry, Water it!" display one column over on the first 
  // row:

  if (sensorValue <= thresholdDown){
    // Now we are going to check if the water level is below a 
    // out thresholdDown value we set earlier, and if it is have 
    // words "Dry, Water it!" display one column over on the second 
    // row:
    DisplayWords = "Dry, Water it!";
    TweetWords = "Feeling a bit parched here, could do with a drink!";
    TweetThreshold -= 1;


  } else if (sensorValue >= thresholdUp){
    // If the value is not below our thresholdDown value we want to 
    // check if it is above our thresholdUp value, and if it is 
    // change the display words to "Wet, Leave it!":

    DisplayWords = "Wet, Leave it !";
    TweetWords = "A little bit damp here, so I don't need any watering thanks!";
    TweetThreshold += 1;


  } else {
    // Otherwise say nothing
    DisplayWords = "";
  }

  Serial.print(DisplayWords);
  lcd.print(DisplayWords);

  if (abs(TweetThreshold) > TweetThresholdWarning){
    Serial.println("TweetThreshold " + String(TweetThreshold));
  }
  
  // only try to send the tweet if we haven't already sent it successfully
  if (abs(TweetThreshold) > TweetThresholdLimit) {
    // Debug information for the User over the computers serial connection
    Serial.println("Tweeting - Run #" + String(TweetWords) + "...");

    // Send out our message!
    tweet(TweetWords);
    
    // Reset the Threshold so we don't repeatedly tweet the same thing!
    TweetThreshold = 0;
  }
  // Wait for a second
  delay(1000);

}

// This function sends a direct message containing the contents of the string "tweetText" to a 
// predefined recipient, set in DM_TARGET
bool tweet(String tweetText){
    TembooChoreo DirectMessageChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    DirectMessageChoreo.begin();
    
    // set Temboo account credentials
    DirectMessageChoreo.setAccountName(TEMBOO_ACCOUNT);
    DirectMessageChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    DirectMessageChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Twitter > DirectMessages > SendDirectMessage)
    DirectMessageChoreo.setChoreo("/Library/Twitter/DirectMessages/SendDirectMessage");
    // set the required choreo inputs
    // see https://temboo.com/library/Library/Twitter/DirectMessages/SendDirectMessage/
    // for complete details about the inputs for this Choreo
 
    // add the Twitter account information
    DirectMessageChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    DirectMessageChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    DirectMessageChoreo.addInput("ConsumerKey", TWITTER_API_KEY);    
    DirectMessageChoreo.addInput("ConsumerSecret", TWITTER_API_SECRET);

    // set the DM text and recipient
    DirectMessageChoreo.addInput("Text", tweetText);
    DirectMessageChoreo.addInput("ScreenName", DM_TARGET);

    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = DirectMessageChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
        Serial.println("Success! Tweet sent!");
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (DirectMessageChoreo.available()) {
        char c = DirectMessageChoreo.read();
        Serial.print(c);
      }
    } 
    DirectMessageChoreo.close();
}


