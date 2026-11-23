#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <INA219_WE.h>
#include <CD74HC4067.h>

#include "pins.h"
#include "display.h"
#include "functions.h"
#include "measure.h"
#include "pitches.h"

///////Variables/////////////

//Encoder

int counter = 0;
int currentStateENC_A;
int lastStateENC_A;
String currentDir = "";
const int SHORT_PRESS_TIME = 1000;  // 1000 milliseconds
const int LONG_PRESS_TIME = 1500;   // 1000 milliseconds
int lastState = LOW;                // the previous state from the input pin
int currentState;                   // the current reading from the input pin
unsigned long presseENC_Bime = 0;
unsigned long releaseENC_Bime = 0;
int ButtonState = 0;  //0 - Not pressed, 1 - short press, 2 - long press

//INA219

INA219_WE ina219 = INA219_WE(INA219_ADRESS);
float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float power_mW = 0.0;
bool ina219_overflow = false;

//CD74HC4067

CD74HC4067 AnalogMulti(S0, S1, S2, S3);
float Cell1 = 0.00;
float Cell2 = 0.00;
float Cell3 = 0.00;
float Cell4 = 0.00;
float Cell5 = 0.00;
float Cell6 = 0.00;

float Cell1AverageArray[8] = { 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20 };
float Cell2AverageArray[8] = { 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20 };
float Cell3AverageArray[8] = { 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20 };
float Cell4AverageArray[8] = { 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20 };
float Cell5AverageArray[8] = { 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20 };
float Cell6AverageArray[8] = { 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20, 4.20 };

int Cell1AverageValue = 0;
int Cell2AverageValue = 0;
int Cell3AverageValue = 0;
int Cell4AverageValue = 0;
int Cell5AverageValue = 0;
int Cell6AverageValue = 0;

float Cell1Average = 0.00;
float Cell2Average = 0.00;
float Cell3Average = 0.00;
float Cell4Average = 0.00;
float Cell5Average = 0.00;
float Cell6Average = 0.00;

///////////////////////////

void CheckRotaryEncoder() {
  currentStateENC_A = digitalRead(ENC_A);
  if (currentStateENC_A != lastStateENC_A && currentStateENC_A == 1) {
    if (digitalRead(ENC_B) != currentStateENC_A) {
      counter--;
      currentDir = "CCW";
    } else {
      counter++;
      currentDir = "CW";
    }
  }

  lastStateENC_A = currentStateENC_A;

  currentState = digitalRead(ENC_SW);

  if (lastState == LOW && currentState == HIGH) {
    presseENC_Bime = millis();
  } else if (lastState == HIGH && currentState == LOW) {
    releaseENC_Bime = millis();

    long pressDuration = releaseENC_Bime - presseENC_Bime;

    if (pressDuration < SHORT_PRESS_TIME) {
      ButtonState = 1;
    } else if (pressDuration > LONG_PRESS_TIME) {
      ButtonState = 2;
    } else {
      ButtonState = 0;
    }
  }
  lastState = currentState;
}

void InitINA219() {
  Wire.begin();
  if (!ina219.init()) {
    Serial.println("INA219 not connected!");
  }
  ina219.setShuntSizeInOhms(0.01);  // Insert your shunt size in ohms
}

void ReadINA219() {
  shuntVoltage_mV = ina219.getShuntVoltage_mV();
  busVoltage_V = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getBusPower();
  loadVoltage_V = busVoltage_V + (shuntVoltage_mV / 1000);
  ina219_overflow = ina219.getOverflow();
  if (!ina219_overflow) {
    Serial.println("Values OK - no overflow");
  } else {
    Serial.println("Overflow! Choose higher PGAIN");
  }
  Serial.println();
}

void ReadCellVoltages() {
  float MultiVoltage;

  ////////////////////////////////////////////
  AnalogMulti.channel(0);
  MultiVoltage = analogRead(BVCC);
  Cell1 = (MultiVoltage - 0) * (4.20 - 0.00) / (4095 - 0) + 0.00;

  Cell1AverageArray[Cell1AverageValue] = Cell1;

  if (Cell1AverageValue == 7) {
    Cell1AverageValue = 0;
  } else {
    Cell1AverageValue = Cell1AverageValue + 1;
  }

  Cell1Average = ((Cell1AverageArray[0] + Cell1AverageArray[1] + Cell1AverageArray[2] + Cell1AverageArray[3] + Cell1AverageArray[4] + Cell1AverageArray[5] + Cell1AverageArray[6] + Cell1AverageArray[7]) / 8) + 0.34; //M:0.34

  ////////////////////////////////////////////

  AnalogMulti.channel(1);
  MultiVoltage = analogRead(BVCC);
  Cell2 = (MultiVoltage - 0) * (4.20 - 0.00) / (4095 - 0) + 0.00;

  Cell2AverageArray[Cell2AverageValue] = Cell2;

  if (Cell2AverageValue == 7) {
    Cell2AverageValue = 0;
  } else {
    Cell2AverageValue = Cell2AverageValue + 1;
  }

  Cell2Average = ((Cell2AverageArray[0] + Cell2AverageArray[1] + Cell2AverageArray[2] + Cell2AverageArray[3] + Cell2AverageArray[4] + Cell2AverageArray[5] + Cell2AverageArray[6] + Cell2AverageArray[7]) / 8) + 0.21; //M:0.21

  ////////////////////////////////////////////

  AnalogMulti.channel(2);
  MultiVoltage = analogRead(BVCC);
  Cell3 = (MultiVoltage - 0) * (4.20 - 0.00) / (4095 - 0) + 0.00;

  Cell3AverageArray[Cell3AverageValue] = Cell3;

  if (Cell3AverageValue == 7) {
    Cell3AverageValue = 0;
  } else {
    Cell3AverageValue = Cell3AverageValue + 1;
  }

  Cell3Average = ((Cell3AverageArray[0] + Cell3AverageArray[1] + Cell3AverageArray[2] + Cell3AverageArray[3] + Cell3AverageArray[4] + Cell3AverageArray[5] + Cell3AverageArray[6] + Cell3AverageArray[7]) / 8) + 0.20; //M:0.20

  ////////////////////////////////////////////

  AnalogMulti.channel(3);
  MultiVoltage = analogRead(BVCC);
  Cell4 = (MultiVoltage - 0) * (4.20 - 0.00) / (4095 - 0) + 0.00;

  Cell4AverageArray[Cell4AverageValue] = Cell4;

  if (Cell4AverageValue == 7) {
    Cell4AverageValue = 0;
  } else {
    Cell4AverageValue = Cell4AverageValue + 1;
  }

  Cell4Average = ((Cell4AverageArray[0] + Cell4AverageArray[1] + Cell4AverageArray[2] + Cell4AverageArray[3] + Cell4AverageArray[4] + Cell4AverageArray[5] + Cell4AverageArray[6] + Cell4AverageArray[7]) / 8) + 0.10; //M:0.10

  ////////////////////////////////////////////
  
  AnalogMulti.channel(4);
  MultiVoltage = analogRead(BVCC);
  Cell5 = (MultiVoltage - 0) * (4.20 - 0.00) / (4095 - 0) + 0.00;

  Cell5AverageArray[Cell5AverageValue] = Cell5;

  if (Cell5AverageValue == 7) {
    Cell5AverageValue = 0;
  } else {
    Cell5AverageValue = Cell5AverageValue + 1;
  }

  Cell5Average = ((Cell5AverageArray[0] + Cell5AverageArray[1] + Cell5AverageArray[2] + Cell5AverageArray[3] + Cell5AverageArray[4] + Cell5AverageArray[5] + Cell5AverageArray[6] + Cell5AverageArray[7]) / 8) + 0.20; //M:0.20

  ////////////////////////////////////////////

  AnalogMulti.channel(5);
  MultiVoltage = analogRead(BVCC);
  Cell6 = (MultiVoltage - 0) * (4.20 - 0.00) / (4095 - 0) + 0.00;

  Cell6AverageArray[Cell6AverageValue] = Cell6;

  if (Cell6AverageValue == 7) {
    Cell6AverageValue = 0;
  } else {
    Cell6AverageValue = Cell6AverageValue + 1;
  }

  Cell6Average = ((Cell6AverageArray[0] + Cell6AverageArray[1] + Cell6AverageArray[2] + Cell6AverageArray[3] + Cell6AverageArray[4] + Cell6AverageArray[5] + Cell6AverageArray[6] + Cell6AverageArray[7]) / 8) + 0.21; //M:0.21
}

/*
float Cell1 = 0.00;
float Cell2 = 0.00;
float Cell3 = 0.00;
float Cell4 = 0.00;
float Cell5 = 0.00;
float Cell6 = 0.00;

float Cell1AverageArray[5] = {4.20, 4.20, 4.20, 4.20, 4.20};
float Cell2AverageArray[5] = {4.20, 4.20, 4.20, 4.20, 4.20};
float Cell3AverageArray[5] = {4.20, 4.20, 4.20, 4.20, 4.20};
float Cell4AverageArray[5] = {4.20, 4.20, 4.20, 4.20, 4.20};
float Cell5AverageArray[5] = {4.20, 4.20, 4.20, 4.20, 4.20};
float Cell6AverageArray[5] = {4.20, 4.20, 4.20, 4.20, 4.20};

int Cell1AverageValue = 0;
int Cell2AverageValue = 0;
int Cell3AverageValue = 0;
int Cell4AverageValue = 0;
int Cell5AverageValue = 0;
int Cell6AverageValue = 0;

float Cell1Average = 0.00;
float Cell2Average = 0.00;
float Cell3Average = 0.00;
float Cell4Average = 0.00;
float Cell5Average = 0.00;
float Cell6Average = 0.00;

*/