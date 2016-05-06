/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//// Tango Reliever Code                                                                     ////
//// Kaho Abe                                                                                ////
//// May 24, 2015                                                                            ////
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

////////////////////////////////////OTHER VARIABLES////////////////////////////////////
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
myChar = mySerial.read();
Serial.println(myChar);
}
  
    if (myChar == NORMAL) { //half glow, half vibe. low flicker lights 
    Serial.println("NORMAL");
    glowWhiteHalf();
    vibeHalf();
   theaterChase(tube.Color(127, 127, 127), 0);
    }
    
    else if (myChar == ACTIVE) {// full glow, high vibe , fast flicker lights 
    Serial.println("active ghostlighton, glowwhitefull, vibefull, theater");
       ghostLightON();
       glowWhiteFull();
      vibeFull();
      theaterChase(tube.Color(127, 127, 127), 0);
    }
    
    else if (myChar == TIMETRAVEL) { //sparkly, high vibe, fast flicker lights
      Serial.println("timetravel ghostlighton, glowwhitefull, vibefull, theater");
      ghostLightON();
      glowWhiteFullSparks();
      vibeFull();
      theaterChase(tube.Color(127, 127, 127), 0);
    }
    
    else if(myChar == HOLD) { //red, high vibe, fast flicker lights
      ghostLightON();
      Serial.println("hold");
      angryRed();
      theaterChase(bulb.Color(127,   0,   0), 0);
      vibeFull();
   
    }
    
    else if (myChar == RELEASE) { // blue, SILENT, lights normal
      Serial.println("Release, TheaterChase");
      ghostLightOFF();
      blue();
     theaterChase(tube.Color(0, 0, 127), 0);
    }
    
    else if (myChar == RED) {
      Serial.println("REd, AngryRed, and Theater");
      angryRed();
      theaterChase(tube.Color(127, 0, 0), 0);
    }
    
    else if (myChar == BLUE) {
         Serial.println("BLUE, TheaterChase");
      theaterChase(tube.Color(0, 0, 127), 0);
    }
    
    else if (myChar == LOWVIBE) {
      Serial.println("Lowvibe");
      vibeHalf();
    }
    
    else if (myChar == HIGHVIBE) {
     Serial.println("Highvibe"); 
      vibeFull();
    }
    else if (myChar == REST) {
       Serial.println("rest"); 
       
    }
  
 }

////////////////////////////////////NEOPIXEL FUNCTIONS////////////////////////////////////

void glowWhiteHalf() {
     brightness = float((sin(counter) +1.0 )/2) ;
  for(uint16_t i=0; i<bulb.numPixels(); i++) {
       bulb.setPixelColor(i, (brightness*63) , (brightness*63), (brightness*63));
       bulb.show();
  }
}

void glowWhiteFull() {
  brightness = float((sin(counter) +1.0 )/2) ;
  for(uint16_t i=0; i<bulb.numPixels(); i++) {
       bulb.setPixelColor(i, (brightness*255) , (brightness*255), (brightness*255));
       bulb.show(); 
  }
}

void glowWhiteFullSparks() {
  if(int(counter)%6 == 0) {
       colorWipe(bulb.Color(random(255), random(255), random(255)),0); // Red 
      // colorWipe(bulb.Color(255, 0, 0), 50); // Red
  }
  brightness = float((sin(counter) +1.0 )/2) ;
  for(uint16_t i=0; i<bulb.numPixels(); i++) {
       bulb.setPixelColor(i, (brightness*255) , (brightness*255), (brightness*255));
       bulb.show();
  }
  
}

void angryRed() {
   brightness = random(255) ;
   for(uint16_t i=0; i<bulb.numPixels(); i++) {    
       bulb.setPixelColor(i, (brightness*255) , 0,0);
       bulb.show();    
  }
   delay(100);
}

void blue() {
   brightness = float((sin(counter) +1.0 )/2) ;
   for(uint16_t i=0; i<bulb.numPixels(); i++) {    
       bulb.setPixelColor(i,  0, 0,(brightness*255));
       bulb.show();    
  }
   delay(100);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<bulb.numPixels(); i++) {
      bulb.setPixelColor(i, c);
      bulb.show();
      delay(wait);
  }
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

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < tube.numPixels(); i=i+3) {
        tube.setPixelColor(i+q, c);    //turn every third pixel on
      }
      tube.show();
     
      delay(wait);
     
     for (int i=0; i < tube.numPixels(); i=i+3) {
       tube.setPixelColor(i+q, 0);        //turn every third pixel off
     }
    }
  }
}


void ghostLightON() {
  if(ghostLightState == false) {
      ghostLightState = true;
  if(int(counter)%50 == 0) {
   
    digitalWrite(LIGHT1, LOW);
     Serial.println("Ghostlight LOW");
  delay(random(50));   
    digitalWrite(LIGHT1, HIGH); 
     Serial.println("Ghostlight HIGH");
    delay(random(50));
    digitalWrite(LIGHT1, LOW); 
    Serial.println("Ghostlight LOW");  
  }
  } 
}

void ghostLightOFF() {
    Serial.println("Ghostlight Off");
   digitalWrite(LIGHT1, LOW); 
   ghostLightState = false;
     
  
}

//boolean cycleCheck(unsigned long *lastMillis, unsigned int cycle)
//{
// unsigned long currentMillis = millis();
// if(currentMillis - *lastMillis >= cycle)
// {
//   *lastMillis = currentMillis;
//   return true;
// }
// else
//   return false;
//}

//void serialEvent() {
//  if (mySerial.available()) {
//    // get the new byte:
//   myChar = (char)mySerial.read(); 
//    // add it to the inputString:
//    
//  }
//}

