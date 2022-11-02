
const int CEPIN = 9;
const int CSNPIN = 10;
const int servoPIN = 5;

#include <Servo.h>
Servo myservo;

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(CEPIN, CSNPIN);                // CE, CSN

const byte address[6] = "401";

void setup() {
  myservo.attach(servoPIN);
  Serial.begin(9600);
  
  // RF24 setup
  if (!radio.begin()) {
    Serial.println("radio  initialization failed");
    while (1);
  } else {
    Serial.println("radio successfully initialized");
  }
  
  radio.openReadingPipe(0, address);  //destination addres
  radio.setPALevel(RF24_PA_MIN);   // min or max
  radio.startListening();
}

void loop() {
  uint8_t pipeNum;
  if (radio.available(&pipeNum))  //Looking for the data.
  {
    Serial.print("data available on pipe ");
    Serial.println(pipeNum);
    
    int data;
    radio.read(&data, sizeof(data));  //Reading the data
    Serial.print("data = ");
    Serial.println(data);

    int angle = map(data, 0, 1023, 0, 180);
    myservo.write(angle);
    delay(15);
  }
}
