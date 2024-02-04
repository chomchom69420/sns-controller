#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>

// Define the digital inputs
#define jB1        3  // Joystick button 1
#define j1POTX_PIN A1
#define j1POTY_PIN A0
#define CE         5
#define CSN        6

const int MPU = 0x68; // MPU6050 I2C address
float elapsedTime, currentTime, previousTime;
int c = 0;

RF24 radio(CE, CSN);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001"; // Address

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
};

Data_Package data; //Create a variable with the above structure

void setup() {
  Serial.begin(115200);
  
  // Define the radio communication
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(3,5); //delay, count
  
  // Activate the Arduino internal pull-up resistors
  pinMode(jB1, INPUT_PULLUP);
  
  // Set initial default values
  data.j1PotX = 127; // Values from 0 to 255. When Joystick is in resting position, the value is in the middle, or 127. We actually map the pot value from 0 to 1023 to 0 to 255 because that's one BYTE value
  data.j1PotY = 127;
  data.j1Button = 1;
}
void loop() {
  // Read all analog inputs and map them to one Byte value
  data.j1PotX = map(analogRead(j1POTX_PIN), 0, 1023, 0, 255); // Convert the analog read value from 0 to 1023 into a BYTE value from 0 to 255
  data.j1PotY = map(analogRead(j1POTY_PIN), 0, 1023, 0, 255);
  // Read all digital inputs
  data.j1Button = digitalRead(jB1);
  Serial.print("Pot X: ");
  Serial.print(data.j1PotX);
  Serial.print("Pot Y: ");
  Serial.print(data.j1PotY);
  Serial.println();
  if (radio.write(&data, sizeof(Data_Package))==true) {
    Serial.println("Write successful");
  }
  else 
    Serial.println("Write failed");
  
  delay(50);
}

