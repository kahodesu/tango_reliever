#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>
//------------------- Spiritus Code Begin---------------------//
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
//#include <SoftwareSerial.h>
//------------------- Spiritus Code End---------------------//
#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack

//------------------- Spiritus Code Begin---------------------//
//#define rxPin 12
//#define txPin 11

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
char stateArray[] = {NORMAL, ACTIVE, TIMETRAVEL, HOLD, RELEASE, RED, BLUE, LOWVIBE, HIGHVIBE, REST}; ////////////////////////////////////OTHER VARIABLES////////////////////////////////////
//Neopixel variables
#define NEOPIN1 8
#define NEOPIN2 9
Adafruit_NeoPixel bulb = Adafruit_NeoPixel(60, NEOPIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel tube = Adafruit_NeoPixel(15, NEOPIN2, NEO_GRB + NEO_KHZ800);
float counter = 0;
float brightness = 0;
int chaseTime = 100; //theater chase time

//Relay variables
int relayPin1 = 4;                 // IN1 connected to digital pin 7
int LIGHT1 = 5;                 // IN2 connected to digital pin 8
int LIGHT2 = 6;                 // IN3 connected to digital pin 9
int VIBE = 7;                // IN4 connected to digital pin 10

int val = 0;


//boolean ghostLightState = 0; //no longer needed?? 

//Light Variables
boolean ghostLightState1 = false;
unsigned long startFlickerTime1;
int randomFlicker1;
boolean flickerOn1;
boolean ghostLightState2 = false;
unsigned long startFlickerTime2;
int randomFlicker2;
boolean flickerOn2;
int flickerFreq = 2000;

//SoftwareSerial mySerial(rxPin, txPin); // RX, TX
char myChar ;t
char tempChar;

//------------------- Spiritus Code End---------------------//
RFM69 radio;
SPIFlash flash(8, 0xEF30); //EF40 for 16mbit windbond chip
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network
int ledpin = 4;
typedef struct {
  int           nodeId; //store this nodeId
  unsigned long uptime; //uptime in ms
  float         temp;   //temperature maybe?
  int          spiritValue; // incoming bluetooth value
} Payload;
Payload theData;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  //------------------- Spiritus Code Begin---------------------//
  //  mySerial.begin(9600);

  bulb.begin();
  bulb.show(); // Initialize all pixels to 'off'
  tube.begin();
  tube.show(); // Initialize all pixels to 'off'

  //Relay set up
  pinMode(relayPin1, OUTPUT);      // sets the digital pin as output
  pinMode(LIGHT1, OUTPUT);      // sets the digital pin as output
  pinMode(LIGHT2, OUTPUT);      // sets the digital pin as output
  pinMode(VIBE, OUTPUT);      // s                            ets the digital pin as output
  digitalWrite(relayPin1, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(LIGHT1, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(LIGHT2, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(VIBE, HIGH);     // Prevents relays from starting up engaged
 
 
  pinMode(ledpin, OUTPUT);      // s                            ets the digital pin as output
  digitalWrite(ledpin, HIGH);
//  ghostLightState = false;  //no longer needed??
  ghostLightON();
  //------------------- Spiritus Code End---------------------//
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  //radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  radio.promiscuous(promiscuousMode);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  if (flash.initialize())
    Serial.println("SPI Flash Init OK!");
  else
    Serial.println("SPI Flash Init FAIL! (is chip present?)");
}

byte ackCount = 0;
void loop() {
//  glowWhiteFull();
//  digitalWrite(ledpin, LOW);
//  delay(100);
//  digitalWrite(ledpin, HIGH);
//  Serial.println("start the party");
  //process any serial input
  if (Serial.available() > 0)
  {

    char input = Serial.read();
    if (input == 'r') //d=dump all register values
      radio.readAllRegs();
    if (input == 'E') //E=enable encryption
      radio.encrypt(KEY);
    if (input == 'e') //e=disable encryption
      radio.encrypt(null);
    if (input == 'p')
    {
      promiscuousMode = !promiscuousMode;
      radio.promiscuous(promiscuousMode);
      Serial.print("Promiscuous mode "); Serial.println(promiscuousMode ? "on" : "off");
    }

    if (input == 'd') //d=dump flash area
    {
      Serial.println("Flash content:");
      int counter = 0;

      while (counter <= 256) {
        Serial.print(flash.readByte(counter++), HEX);
        Serial.print('.');
      }
      while (flash.busy());
      Serial.println();
    }
    if (input == 'D')
    {
      Serial.print("Deleting Flash chip content... ");
      flash.chipErase();
      while (flash.busy());
      Serial.println("DONE");
    }
    if (input == 'i')
    {
      Serial.print("DeviceID: ");
      word jedecid = flash.readDeviceId();
      Serial.println(jedecid, HEX);
    }
    if (input == '9')
    {
      //      Blink(ledpin, 1000);
    }
  }

  if (radio.receiveDone())
  {
    Serial.print('['); Serial.print(radio.SENDERID, DEC); Serial.print("] ");
    Serial.print(" [RX_RSSI:"); Serial.print(radio.readRSSI()); Serial.print("]");
    if (promiscuousMode)
    {
      Serial.print("to ["); Serial.print(radio.TARGETID, DEC); Serial.print("] ");
    }

    if (radio.DATALEN != sizeof(Payload))
      Serial.print("Invalid payload received, not matching Payload struct!");
    else
    {
      theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
      Serial.print(" nodeId=");
      Serial.print(theData.nodeId);
      Serial.print(" uptime=");
      Serial.print(theData.uptime);
      Serial.print(" temp=");
      Serial.print(theData.temp);
      Serial.print(" spiritValue = ");
      Serial.println(theData.spiritValue);
      if (theData.spiritValue) {
        spiritValueHanding(theData.spiritValue);
//        glowWhiteFull();
//        digitalWrite(ledpin, LOW);
//        delay(100);
//        digitalWrite(ledpin, HIGH);
        Serial.write("start the party");
       }

    }

    if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      Serial.print(" - ACK sent.");
      //      Blink(ledpin,100);
      // When a node requests an ACK, respond to the ACK
      // and also send a packet requesting an ACK (every 3rd one only)
      // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
      if (ackCount++ % 3 == 0)
      {
        Serial.print(" Pinging node ");
        Serial.print(theNodeID);
        Serial.print(" - ACK...");
        delay(3); //need this when sending right after reception .. ?
        if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0))  // 0 = only 1 attempt, no retries
          Serial.print("ok!");
        else Serial.print("nothing");
      }
    }
    Serial.println();
    Blink(LED, 3);
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN, LOW);
}
//------------------- Spiritus Code Begin---------------------//
void spiritValueHanding(char spiritValue) {

  if (spiritValue) {
    tempChar = spiritValue;
    for (int i = 0; i < sizeof(stateArray); i++) {
      if (tempChar == stateArray[i]) {
        myChar = tempChar;
      } else {
        //      Serial.print("tempChar: ");
        //      Serial.println(tempChar);
      }
    }
    Serial.print("myChar: ");
    Serial.println(myChar);
  }

  if (myChar == NORMAL) { //half glow, half vibe. low flicker lights
    Serial.println("NORMAL");
    glowWhiteHalf();
    vibeHalf();
    theaterChase(tube.Color(127, 127, 127), chaseTime);
  }

  else if (myChar == ACTIVE) {// full glow, high vibe , fast flicker lights
    Serial.println("active ghostlighton, glowwhitefull, vibefull, theater");
    ghostLightON();
    glowWhiteFull();
    vibeFull();
    theaterChase(tube.Color(127, 127, 127), chaseTime);
  }

  else if (myChar == TIMETRAVEL) { //sparkly, high vibe, fast flicker lights
    Serial.println("timetravel ghostlighton, glowwhitefull, vibefull, theater");
    ghostLightON();
    glowWhiteFullSparks();
    vibeFull();
    theaterChase(tube.Color(127, 127, 127), chaseTime);
  }

  else if (myChar == HOLD) { //red, high vibe, fast flicker lights
    ghostLightON();
    Serial.println("hold");
    angryRed();
    theaterChase(bulb.Color(127,   0,   0), chaseTime);
    vibeFull();

  }

  else if (myChar == RELEASE) { // blue, SILENT, lights normal
    Serial.println("Release, TheaterChase");
    ghostLightOFF();
    blue();
    theaterChase(tube.Color(0, 0, 127), chaseTime);
  }

  else if (myChar == RED) {
    Serial.println("REd, AngryRed, and Theater");
    angryRed();
    theaterChase(tube.Color(127, 0, 0), chaseTime);
  }

  else if (myChar == BLUE) {
    Serial.println("BLUE, TheaterChase");
    theaterChase(tube.Color(0, 0, 127), chaseTime);
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
    Serial.println(": rest");
    glowWhiteHalf();
  }

}



void glowWhiteHalf() {
  brightness = float((sin(counter) + 1.0 ) / 2) ;
  for (uint16_t i = 0; i < bulb.numPixels(); i++) {
    bulb.setPixelColor(i, (brightness * 63) , (brightness * 63), (brightness * 63));
    bulb.show();
  }
}

void glowWhiteFull() {
  Serial.println("party mode");
  brightness = float((sin(counter) + 1.0 ) / 2) ;
  for (uint16_t i = 0; i < bulb.numPixels(); i++) {
    bulb.setPixelColor(i, (brightness * 255) , (brightness * 255), (brightness * 255));
    bulb.show();
  }
}

void glowWhiteFullSparks() {
  if (int(counter) % 6 == 0) {
    colorWipe(bulb.Color(random(255), random(255), random(255)), 0); // Red
    // colorWipe(bulb.Color(255, 0, 0), 50); // Red
  }
  brightness = float((sin(counter) + 1.0 ) / 2) ;
  for (uint16_t i = 0; i < bulb.numPixels(); i++) {
    bulb.setPixelColor(i, (brightness * 255) , (brightness * 255), (brightness * 255));
    bulb.show();
  }

}

void angryRed() {
  Serial.println('grrrrrrrrrr');
  brightness = random(255) ;
  for (uint16_t i = 0; i < bulb.numPixels(); i++) {
    bulb.setPixelColor(i, (brightness * 255) , 0, 0);
    bulb.show();
  }
  delay(100);
}

void blue() {

  brightness = float((sin(counter) + 1.0 ) / 2) ;
  for (uint16_t i = 0; i < bulb.numPixels(); i++) {
    bulb.setPixelColor(i,  0, 0, (brightness * 255));
    bulb.show();
  }
  delay(100);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < bulb.numPixels(); i++) {
    bulb.setPixelColor(i, c);
    bulb.show();
    delay(wait);
  }
}




void vibeFull() {
  digitalWrite(VIBE, LOW);   // energizes the relay and lights the LED
  digitalWrite(relayPin1, LOW);   // energizes the relay and lights the LED

  delay(6);
  digitalWrite(relayPin1, HIGH);
}


void vibeHalf() {
  digitalWrite(VIBE, LOW);   // energizes the relay and lights the LED
  delay(50);
  digitalWrite(VIBE, HIGH);
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < tube.numPixels(); i = i + 3) {
        tube.setPixelColor(i + q, c);  //turn every third pixel on
      }
      tube.show();

      delay(wait);

      for (int i = 0; i < tube.numPixels(); i = i + 3) {
        tube.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}


void ghostLightON() {
   
/////////////// FLICKERING FOR LIGHT 1 ///////////////
  if(ghostLightState1 == false) {
    ghostLightState1 = true;
    startFlickerTime1 = millis();
    randomFlicker1 = int(random(500, flickerFreq));
  }

  if (millis() - startFlickerTime1 > randomFlicker1) {
    flickerOn1 = !flickerOn1;
    ghostLightState1 = false;
  }

  if (flickerOn1) {
    digitalWrite(LIGHT1, HIGH); 
  }

  else {
    digitalWrite(LIGHT1, LOW); 
  }  
  
  /////////////// FLICKERING FOR LIGHT 2 ///////////////
    if(ghostLightState2 == false) {
    ghostLightState2 = true;
    startFlickerTime2 = millis();
    randomFlicker2 = int(random(500, flickerFreq));
  }

  if (millis() - startFlickerTime2 > randomFlicker2) {
    flickerOn2 = !flickerOn2;
    ghostLightState2 = false;
  }

  if (flickerOn2) {
    digitalWrite(LIGHT2, HIGH); 
  }

  else {
    digitalWrite(LIGHT2, LOW); 
  }
}

void ghostLightOFF() {
  Serial.println("Ghostlight Off");
  digitalWrite(LIGHT1, HIGH); 
  digitalWrite(LIGHT2, HIGH); 
  ghostLightState1 = false;
  ghostLightState2 = false;
}
//------------------- Spiritus Code End---------------------//
