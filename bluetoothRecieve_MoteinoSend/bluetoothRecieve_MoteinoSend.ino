#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>
#include <SoftwareSerial.h>

SoftwareSerial BluetoothSerial(5, 6);
#define NODEID      99
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack

int TRANSMITPERIOD = 300; //transmit a packet to gateway so often (in ms)
byte sendSize = 0;
boolean requestACK = false;
SPIFlash flash(8, 0xEF30); //EF40 for 16mbit windbond chip
RFM69 radio;
int buttonPin = 3;
int value = -1;
int previousValue = -1;
int btInput = 0;
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorPin1 = A2;
int sensorValue = 0;  // variable to store the value coming from the sensor
int sensorValue1 = 0;
int analogBaseValue = 1010;
char temp;

typedef struct {
  int           nodeId; //store this nodeId
  unsigned long uptime; //uptime in ms
  float         temp;   //temperature maybe?
  int spiritValue; // incoming value from Tango
} Payload;
Payload theData;

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  //radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

  if (flash.initialize())
    Serial.println("SPI Flash Init OK!");
  else
    Serial.println("SPI Flash Init FAIL! (is chip present?)");
  BluetoothSerial.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  BluetoothSerial.print("$$$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  BluetoothSerial.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  BluetoothSerial.begin(9600);  // Start bluetooth serial at 9600
}

long lastPeriod = -1;
void loop() {
  // Read Analog values
  sensorValue = analogRead(sensorPin);
  sensorValue1 = analogRead(sensorPin1);
  //if theres a change then send the values to the Tango over BT
  if (sensorValue < analogBaseValue || sensorValue1 < analogBaseValue) {
    Serial.print(sensorValue);
    Serial.print(",");
    Serial.println(sensorValue1);
    BluetoothSerial.println("halfhold");
    if (sensorValue < analogBaseValue && sensorValue1 < analogBaseValue) {

      Serial.print("sending charaters over BT: fullhold");
      // Send any characters the Serial monitor prints to the bluetooth
      BluetoothSerial.println("fullhold");
    }
  }
  /// Incoming Bluetooth data from arduino
  if (BluetoothSerial.available()) { // If the bluetooth sent any characters
    char temp = BluetoothSerial.read();
    if (temp != 10) {
      btInput = temp;
      Serial.print("temp available: ");
      Serial.println(temp);
    }
    // Send any characters the bluetooth prints to the serial monitor
    Serial.print("character available: ");
    Serial.println(btInput);

    int currPeriod = millis() / TRANSMITPERIOD;
    if (currPeriod != lastPeriod) {
      //fill in the struct with new values
      theData.nodeId = NODEID;
      theData.uptime = millis();
      theData.temp = 91.23; //it's hot!
      theData.spiritValue = btInput; //added for income bluetooth data

      Serial.print("Sending struct (");
      Serial.print(sizeof(theData));
      Serial.print(" bytes) ... ");
      if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData)))
        Serial.print(" ok!");
      else Serial.print(" nothing...");
      Serial.println();
      Blink(LED, 3);
      lastPeriod = currPeriod;
    }
  }
  // If stuff was typed in the serial monitor
  //  if (Serial.available()) {
  //    temp = (char)Serial.read();
  //    Serial.println("sending charaters over BT");
  //    // Send any characters the Serial monitor prints to the bluetooth
  //    BluetoothSerial.println(temp);
  //    Serial.println(temp);
  //  }
  //process any serial input
  if (Serial.available() > 0 )  {
    char input = Serial.read();
    if (input >= 48 && input <= 57) //[0,9]
    {
      TRANSMITPERIOD = 100 * (input - 48);
      if (TRANSMITPERIOD == 0) TRANSMITPERIOD = 1000;
      Serial.print("\nChanging delay to ");
      Serial.print(TRANSMITPERIOD);
      Serial.println("ms\n");
    }

    if (input == 'r') //d=dump register values
      radio.readAllRegs();
    //if (input == 'E') //E=enable encryption
    //  radio.encrypt(KEY);
    //if (input == 'e') //e=disable encryption
    //  radio.encrypt(null);

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
    if (input == 'e')
    {
      Serial.print("Erasing Flash chip ... ");
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
  }

  //check for any received packets
  if (radio.receiveDone()) {
    Serial.print('['); Serial.print(radio.SENDERID, DEC); Serial.print("] ");
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:"); Serial.print(radio.readRSSI()); Serial.print("]");

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.print(" - ACK sent");
      delay(10);
    }
    Blink(LED, 5);
    Serial.println();
  }

  int currPeriod = millis() / TRANSMITPERIOD;
  if (currPeriod != lastPeriod) {
    //fill in the struct with new values
    theData.nodeId = NODEID;
    theData.uptime = millis();
    theData.temp = 91.23; //it's hot!
    theData.spiritValue = btInput;

    Serial.print("Sending struct (");
    Serial.print(sizeof(theData));
    Serial.print(" bytes) ... ");
    if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData)))
      Serial.print(" ok!");
    else Serial.print(" nothing...");
    Serial.println();
    Blink(LED, 3);
    lastPeriod = currPeriod;
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN, LOW);
}
