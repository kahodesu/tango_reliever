/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Tango Reliever Code                                                                     ////
//// Kaho Abe                                                                                ////
//// Dec 4, 2015                                                                            ////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// Based on:                                                                                 ////
// Adafruit Neopixels Simple Example                                                         ////
// Sainsmart & Arduino:                                                                      ////
// http://arduinotronics.blogspot.com/2013/01/working-with-sainsmart-5v-relay-board.html     ////
/////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////LIBRARIES///////////////////////////////
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <SoftwareSerial.h>
 
////////////////////////////////////STATES////////////////////////////////////
char NORMAL = '0'; //half glow, half vibe. low flicker lights
char ACTIVE = '1'; // full glow, high vibe , fast flicker lights
char TIMETRAVEL = '2'; //sparkly, high vibe, fast flicker lights
char HOLD = '3'; //red, high vibe, fast flicker lights
char RELEASE = '4'; // blue, SILENT, lights normal
char RED = '5'; 
char BLUE = '6'; 
char LOWVIBE = '7'; 
char HIGHVIBE = '8'; 
char REST = '9'; // white, vibe off, lights normal
char stateArray[] = {NORMAL,ACTIVE,TIMETRAVEL,HOLD,RELEASE,RED,BLUE,LOWVIBE,HIGHVIBE,REST};////////////////////////////////////OTHER VARIABLES////////////////////////////////////
//Neopixel variables
#define NEOPIN1 8
#define NEOPIN2 9
Adafruit_NeoPixel bulb = Adafruit_NeoPixel(60, NEOPIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel tube = Adafruit_NeoPixel(15, NEOPIN2, NEO_GRB + NEO_KHZ800);
float counter = 0;                                                                                                                                                                             
float brightness = 0;

//Relay variables
int relayPin1 = 4;                 // IN1 connected to digital pin 7
int LIGHT1 = 5;                 // IN2 connected to digital pin 8
int LIGHT2 = 6;                 // IN3 connected to digital pin 9
int VIBE = 7;                // IN4 connected to digital pin 10

int val = 0;

#define rxPin 12
#define txPin 11

boolean ghostLightState =0;

SoftwareSerial mySerial(rxPin, txPin); // RX, TX
char myChar ;
char tempChar;
////////////////////////////////////SET UP////////////////////////////////////
void setup()
{
  Serial.begin(9600);
 mySerial.begin(9600);
  
  bulb.begin();
  bulb.show(); // Initialize all pixels to 'off'
  tube.begin();
  tube.show(); // Initialize all pixels to 'off'
  
  //Relay set up
//  pinMode(relayPin1, OUTPUT);      // sets the digital pin as output
  pinMode(LIGHT1, OUTPUT);      // sets the digital pin as output
  pinMode(LIGHT2, OUTPUT);      // sets the digital pin as output
  pinMode(VIBE, OUTPUT);      // s                            ets the digital pin as output
//  digitalWrite(relayPin1, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(LIGHT1, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(LIGHT2, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(VIBE, HIGH);        // Prevents relays from starting up engaged
 ghostLightState = false;
 ghostLightON();

}

////////////////////////////////////LOOP////////////////////////////////////
void loop()
{
  counter= counter + 0.25;
 // while (Serial.available()) {
//  myChar = Serial.read();
//Serial.print(myChar);



if (mySerial.available()) {
  tempChar = mySerial.read();
  for (int i = 0; i < sizeof(stateArray); i++){
    if (tempChar == stateArray[i]){
      myChar = tempChar;
    } else {
//      Serial.print("tempChar: ");
//      Serial.println(tempChar);
    }
  }

Serial.print("myChar:");
Serial.println(myChar);
}
 if(Serial.available())  // If stuff was typed in the serial monitor
  {
    Serial.println("sending charaters over BT");
    // Send any characters the Serial monitor prints to the bluetooth
    mySerial.println((char)Serial.read());
    mySerial.write((char)Serial.read());
    Serial.println(mySerial.write("hey0"));
    Serial.write(mySerial.println("hey0"));
  }
//Serial.println(mySerial.print(myChar));
//Serial.println(mySerial.write("hey"));
//    if (myChar == NORMAL) { //half glow, half vibe. low flicker lights 
//    Serial.println("NORMAL");
//
//    }
//    
//    else if (myChar == ACTIVE) {// full glow, high vibe , fast flicker lights 
//    Serial.println("active ghostlighton, glowwhitefull, vibefull, theater");
//    
//    }
//    
//    else if (myChar == TIMETRAVEL) { //sparkly, high vibe, fast flicker lights
//      Serial.println("timetravel ghostlighton, glowwhitefull, vibefull, theater");
//    
//    }
//    
//    else if(myChar == HOLD) { //red, high vibe, fast flicker lights
//
//      Serial.println("hold");
//     
//   
//    }
//    
//    else if (myChar == RELEASE) { // blue, SILENT, lights normal
//      Serial.println("Release, TheaterChase");
//    }
//    
//    else if (myChar == RED) {
//      Serial.println("REd, AngryRed, and Theater");
//  
//    }
//    
//    else if (myChar == BLUE) {
//         Serial.println("BLUE, TheaterChase");
//   
//    }
//    
//    else if (myChar == LOWVIBE) {
//      Serial.println("Lowvibe");
//
//    }
//    
//    else if (myChar == HIGHVIBE) {
//     Serial.println("Highvibe"); 
//
//    }
//    else if (myChar == REST) {
//       Serial.println(": rest"); 
//    
//    }
  
 }




void vibeFull(){
 
}


void vibeHalf(){
 
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {

}


void ghostLightON() {

}

void ghostLightOFF() {
  
}
