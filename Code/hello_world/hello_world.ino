/*
  Hello_world
 Based on DigitalReadSerial example to print "Hello World!" to the serial interface every second

 This example code is in the public domain.
 */
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // print out Hello World!
  Serial.println("Hello World!");
  delay(1000);        // delay in between reads for stability
}



