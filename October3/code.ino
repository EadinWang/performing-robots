#include <EnableInterrupt.h>

#define SERIAL_PORT_SPEED 9600
#define RC_NUM_CHANNELS  4

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3

#define RC_CH1_PIN  8
#define RC_CH2_PIN  7
#define RC_CH3_PIN  4
#define RC_CH4_PIN  2

#define motorL1_PIN 6
#define motorL2_PIN 5
#define motorR1_PIN 3
#define motorR2_PIN 9

float motor_val_f;
float motor_val_b;

uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

void rc_read_values() {
  noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
  interrupts();
}

void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
  }
}

void calc_ch1() {
  calc_input(RC_CH1, RC_CH1_PIN);
}
void calc_ch2() {
  calc_input(RC_CH2, RC_CH2_PIN);
}
void calc_ch3() {
  calc_input(RC_CH3, RC_CH3_PIN);
}
void calc_ch4() {
  calc_input(RC_CH4, RC_CH4_PIN);
}

void setup() {
  Serial.begin(SERIAL_PORT_SPEED);

  pinMode(RC_CH1_PIN, INPUT);
  pinMode(RC_CH2_PIN, INPUT);
  pinMode(RC_CH3_PIN, INPUT);
  pinMode(RC_CH4_PIN, INPUT);

  enableInterrupt(RC_CH1_PIN, calc_ch1, CHANGE);
  enableInterrupt(RC_CH2_PIN, calc_ch2, CHANGE);
  enableInterrupt(RC_CH3_PIN, calc_ch3, CHANGE);
  enableInterrupt(RC_CH4_PIN, calc_ch4, CHANGE);

  pinMode(motorL1_PIN, OUTPUT);
  pinMode(motorL2_PIN, OUTPUT);
  pinMode(motorR1_PIN, OUTPUT);
  pinMode(motorR2_PIN, OUTPUT);
}

void loop() {
  rc_read_values();
  motor_val_f = map(rc_values[RC_CH1], 1500, 2050, 20, 255);
  motor_val_b = map(rc_values[RC_CH1], 1000, 1500, 255, 20);
  Serial.print("CH1:"); Serial.print(rc_values[RC_CH1]); Serial.print("\t");
  Serial.print("CH2:"); Serial.print(rc_values[RC_CH2]); Serial.print("\t");
  Serial.print("CH3:"); Serial.print(rc_values[RC_CH3]); Serial.print("\t");
  Serial.print("CH4:"); Serial.println(rc_values[RC_CH4]);

  //control the motor
  if (rc_values[RC_CH1] >= 1500) {
    //left
    analogWrite(motorL2_PIN, motor_val_f);
    analogWrite(motorL1_PIN, 20);
    //right
    analogWrite(motorR2_PIN, motor_val_f);
    analogWrite(motorR1_PIN, 20);
  } else {
    //left
    analogWrite(motorL2_PIN, 20);
    analogWrite(motorL1_PIN, motor_val_b);
    //right
    analogWrite(motorR2_PIN, 20);
    analogWrite(motorR1_PIN, motor_val_b);
  }
}
