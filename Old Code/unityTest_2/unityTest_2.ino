#include <SoftwareSerial.h>

SoftwareSerial BluetoothSerial(11,10);

int buttonPin=3;
int value=-1;
int previousValue=-1;
char temp;

void setup()
{
  pinMode(buttonPin, INPUT);

  Serial.begin(9600);
  BluetoothSerial.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  BluetoothSerial.print("$$$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  BluetoothSerial.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  BluetoothSerial.begin(9600);  // Start bluetooth serial at 9600
}

void loop()
{
  value = digitalRead(buttonPin);
//   BluetoothSerial.println('hey');
  if(Serial.available()){
    temp = (char)Serial.read();
    Serial.print("you just send: ");
    Serial.println(temp);
    BluetoothSerial.println(temp);
  };
//  if(value != previousValue){
//    Serial.println(value);
//    BluetoothSerial.println(value);
//    previousValue = value;
//  }
  
  if(BluetoothSerial.available()){
//    Serial.println("!");
    Serial.println((char)BluetoothSerial.read());
  }

  delay(5);
}
