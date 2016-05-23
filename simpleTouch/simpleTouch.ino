//starting point for two touch switches, 
//using bodies to create circuits. 

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorPin1 = A1; 
//int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
int sensorValue1 = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  sensorValue1 = analogRead(sensorPin1);
  // turn the ledPin on
  Serial.print(sensorValue);
  Serial.print(",");
  Serial.println(sensorValue1);
}
