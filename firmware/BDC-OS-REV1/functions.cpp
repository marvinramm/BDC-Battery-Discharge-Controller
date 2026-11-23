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

////////////Variables//////////////

#define MOSFET1_CH 1
#define MOSFET1_ACCURACY 13
#define MOSFET1_FREQ 500

int DischargeState = 0;  // 0 - idle, 1 - discharging, 2 - done
bool EnDischarge = false;

TaskHandle_t Task1;

/////////////////////////////////

void HardwareInit() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(B1_ON, OUTPUT);

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  pinMode(ENC_SW, INPUT);
  pinMode(BVCC, INPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);

  //ledcSetup(MOSFET1_CH, MOSFET1_FREQ, MOSFET1_ACCURACY);
  //ledcAttachPin(B1_ON, MOSFET1_CH);

  DisplayInit();

  InitINA219();

  DrawBootPage();

  lastStateENC_A = digitalRead(ENC_A);


  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */


  //tone(BUZZER, NOTE_A5, 500);
  //tone(BUZZER, NOTE_C4, 300);
}

void Task1code( void * pvParameters ){

  for(;;){
    ReadCellVoltages();
    CheckRotaryEncoder();
    vTaskDelay(10);
  } 
}


void SetPage() {
  if (counter <= 0) {
    counter = 0;
  }

  if (PageNumber == 0) {  //Main Page
    if (counter > 1) {
      counter = 1;
    }
    MainPageNumber = counter;

    if (ButtonState == 1) {

      if (EnDischarge == 0) {
        EnDischarge = 1;
      } else {
        EnDischarge = 0;
      }

      ButtonState = 0;
    } else if (ButtonState == 2) {
      PageNumber = 1;
      ButtonState = 0;
    }

  } else {  //Menu
    if (counter > 1) {
      counter = 1;
    }
    MenuPageNumber = counter;

    if (ButtonState == 1) {
      // TODO: select menu item
      ButtonState = 0;
    } else if (ButtonState == 2) {
      PageNumber = 0;
      ButtonState = 0;
    }
  }
}

void ledcAnalogWrite(int channel, int value, int maxvalue) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  int duty = (8191 / maxvalue) * min(value, maxvalue);

  // write duty to LEDC
  ledcWrite(channel, duty);
}

void SetBatteryLoad() {

  if (DischargeState == 0) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
  } else if (DischargeState == 1) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
  } else {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
  }

  if (EnDischarge == true) {
    digitalWrite(FAN, HIGH);

    if (Cell2Average < 3.73) {  // Stop discharging at the storage voltage
      EnDischarge = false;
      DischargeState = 2;
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(FAN, HIGH);
      //tone(BUZZER, NOTE_A5, 2000);

    } else {

      //      if ((Cell5 > 3.50) && (Cell6 < 3.50)) {
      //        ledcAnalogWrite(MOSFET1_CH, 70, 255);
      //      } else if ((Cell5 > 3.50) && (Cell6 > 3.50)) {
      //        ledcAnalogWrite(MOSFET1_CH, 25, 255);
      //      }else if ((Cell5 < 3.50) && (Cell6 > 3.50)) {
      //        ledcAnalogWrite(MOSFET1_CH, 15, 255);
      //      }else {
      //        ledcAnalogWrite(MOSFET1_CH, 255, 255);
      //      }

      digitalWrite(B1_ON, HIGH);

      DischargeState = 1;
    }
  } else {
    //ledcAnalogWrite(MOSFET1_CH, 0, 255);
    digitalWrite(B1_ON, LOW);
  }
}

void PrintDataSerial() {

  Serial.print("Shunt Voltage [mV]: ");
  Serial.println(shuntVoltage_mV);
  Serial.print("Bus Voltage [V]: ");
  Serial.println(busVoltage_V);
  Serial.print("Load Voltage [V]: ");
  Serial.println(loadVoltage_V);
  Serial.print("Current[mA]: ");
  Serial.println(current_mA);
  Serial.print("Bus Power [mW]: ");
  Serial.println(power_mW);

  Serial.println();

  Serial.print("Direction: ");
  Serial.println(currentDir);
  Serial.print("Counter: ");
  Serial.println(counter);
  Serial.print("Button state:");
  Serial.println(ButtonState);

  Serial.println("______________________");
}