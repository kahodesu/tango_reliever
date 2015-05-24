/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Tango Reliever Code                                                                     ////
//// Kaho Abe                                                                                ////
//// May 24, 2015                                                                            ////
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Based on:                                                                               ////
//// Adafruit Neopixels Simple Example                                                       ////
//// Sainsmart & Arduino:                                                                    ////
//// http://arduinotronics.blogspot.com/2013/01/working-with-sainsmart-5v-relay-board.html   ////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////LIBRARIES///////////////////////////////
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <SoftwareSerial.h>
 
////////////////////////////////////STATES////////////////////////////////////
char  NORMAL = '0'; //half glow, half vibe. low flicker lights
char ACTIVE = '1'; // full glow, high vibe , fast flicker lights
char TIMETRAVEL = '2'; //sparkly, high vibe, fast flicker lights
char HOLD = '3'; //red, high vibe, fast flicker lights
char RELEASE = '4'; // blue, SILENT, lights normal


////////////////////////////////////OTHER VARIABLES////////////////////////////////////
//Neopixel variables
#define PIN 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
float counter = 0;                                                                                                                                                                             
float brightness = 0;

//Relay variables
int relayPin1 = 4;                 // IN1 connected to digital pin 7
int LIGHT1 = 5;                 // IN2 connected to digital pin 8
int LIGHT2 = 6;                 // IN3 connected to digital pin 9
int VIBE = 7;                // IN4 connected to digital pin 10

int val = 0;

#define rxPin 13
#define txPin 12

SoftwareSerial mySerial(rxPin, txPin); // RX, TX
char myChar ;
////////////////////////////////////SET UP////////////////////////////////////
void setup()
{
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  //Relay set up
  pinMode(relayPin1, OUTPUT);      // sets the digital pin as output
  pinMode(LIGHT1, OUTPUT);      // sets the digital pin as output
  pinMode(LIGHT2, OUTPUT);      // sets the digital pin as output
  pinMode(VIBE, OUTPUT);      // s                            ets the digital pin as output
  digitalWrite(relayPin1, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(LIGHT1, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(LIGHT2, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(VIBE, HIGH);        // Prevents relays from starting up engaged
 
   mySerial.begin(9600);

}

////////////////////////////////////LOOP////////////////////////////////////
void loop()
{

  counter= counter + 0.25;
  while (mySerial.available()) {
    myChar = mySerial.read();
  }

  if (myChar == NORMAL) { //half glow, half vibe. low flicker lights 
  glowWhiteHalf();
  vibeHalf();
  }
  
  else if (myChar == ACTIVE) {// full glow, high vibe , fast flicker lights 
    glowWhiteFull();
    vibeFull();
  }
  
  else if (myChar == TIMETRAVEL) { //sparkly, high vibe, fast flicker lights
    glowWhiteFullSparks();
    vibeFull();
  }
  
  else if(myChar == HOLD) { //red, high vibe, fast flicker lights
   angryRed();
   vibeFull();
  }
  
  else if (myChar == RELEASE) { // blue, SILENT, lights normal
   blue();
   digitalWrite(VIBE, HIGH);
  }
//  digitalWrite(relayPin1, LOW);   // energizes the relay and lights the LED
//  digitalWrite(LIGHT1, LOW);   // energizes the relay and lights the LED
//  digitalWrite(LIGHT2, LOW);   // energizes the relay and lights the LED
//  digitalWrite(VIBE, LOW);   // energizes the relay and lights the LED
//  delay(5000);                  // waits for a second
//  digitalWrite(relayPin1, HIGH);    // de-energizes the relay and LED is off
//  digitalWrite(LIGHT1, HIGH);    // de-energizes the relay and LED is off
//  digitalWrite(LIGHT2, HIGH);    // de-energizes the relay and LED is off
//  digitalWrite(VIBE, HIGH);    // de-energizes the relay and LED is off
//  delay(1000);                  // waits for a second
   // Some example procedures showing how to display to the pixels:
//     colorWipe(strip.Color(255, 0, 0), 50); // Red
//  colorWipe(strip.Color(255, 0, 0), 50); // Red
//  colorWipe(strip.Color(0, 255, 0), 50); // Green
//  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//  // Send a theater pixel chase in...
//  theaterChase(strip.Color(127, 127, 127), 50); // White
//  theaterChase(strip.Color(127,   0,   0), 50); // Red
//  theaterChase(strip.Color(  0,   0, 127), 50); // Blue

//  rainbow(20);
//  rainbowCycle(20);
//  theaterChaseRainbow(50);
}


////////////////////////////////////NEOPIXEL FUNCTIONS////////////////////////////////////

void glowWhiteHalf() {
     brightness = float((sin(counter) +1.0 )/2) ;
  for(uint16_t i=0; i<strip.numPixels(); i++) {
       strip.setPixelColor(i, (brightness*63) , (brightness*63), (brightness*63));
       strip.show();
  }
}

void glowWhiteFull() {
  brightness = float((sin(counter) +1.0 )/2) ;
  for(uint16_t i=0; i<strip.numPixels(); i++) {
       strip.setPixelColor(i, (brightness*255) , (brightness*255), (brightness*255));
       strip.show(); 
  }
}

void glowWhiteFullSparks() {
  brightness = float((sin(counter) +1.0 )/2) ;
  for(uint16_t i=0; i<strip.numPixels(); i++) {
       strip.setPixelColor(i, (brightness*255) , (brightness*255), (brightness*255));
       strip.show();
  }
  if(int(counter)%12 == 0) {
       colorWipe(strip.Color(random(255), random(255), random(255)),0); // Red 
      // colorWipe(strip.Color(255, 0, 0), 50); // Red
  }
}

void angryRed() {
   brightness = random(255) ;
   for(uint16_t i=0; i<strip.numPixels(); i++) {    
       strip.setPixelColor(i, (brightness*255) , 0,0);
       strip.show();    
  }
   delay(100);
}

void blue() {
   brightness = float((sin(counter) +1.0 )/2) ;
   for(uint16_t i=0; i<strip.numPixels(); i++) {    
       strip.setPixelColor(i,  0, 0,(brightness*255));
       strip.show();    
  }
   delay(100);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}



void serialEvent() {
  while (Serial.available()) {
    int val = (int)Serial.read();
  }
  Serial.print(val);//FOR DEBUG
}

void vibeFull(){
 digitalWrite(VIBE, LOW);   // energizes the relay and lights the LED
 digitalWrite(relayPin1, LOW);   // energizes the relay and lights the LED
 delay(6);
  digitalWrite(relayPin1, HIGH); 
}


void vibeHalf(){
 digitalWrite(VIBE, LOW);   // energizes the relay and lights the LED
  delay(50);
  digitalWrite(VIBE, HIGH); 
}









