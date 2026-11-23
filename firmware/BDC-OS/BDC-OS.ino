/*
 * BDC OS 0.5.3 - firmware for the Battery Discharge Controller (BDC) REV2
 * https://github.com/marvinramm/BDC
 *
 * Copyright (c) 2022-2023 Marvin Ramm
 * SPDX-License-Identifier: MIT
 *
 * Board: "ESP32 Dev Module", Partition Scheme: "Default 4MB with spiffs"
 *
 * Open points:
 *   - control the load with PWM
 *   - enable OTA updates when the button is held while the battery is plugged in
 *   - discharge exactly to the target voltage with a PID controller
 */

///////////////////Includes//////////////////

#include "pins.h"
#include "display.h"
#include "functions.h"
#include "measure.h"
#include "pitches.h"
#include "settings.h"
#include "ESP32_BLE.h"

//////////////////Variables//////////////////

long PreviousMillis;

/////////////////////////////////////////////

////////////////////Setup////////////////////

void setup(void) {
  HardwareInit();

}

////////////////////LOOP/////////////////////

void loop() {

  ButtonAction();
  Discharge();
  SetLED();

  if ((millis() - PreviousMillis >= 500) || (ForceDisplayUpdate == true)) { //Every 500ms
    PreviousMillis = millis();

    CalculateBATVoltage();
    UpdateDisplay();
    BLELoop();

    //PrintDataSerial();

    ForceDisplayUpdate = false;
  }

  if ((millis() - PreviousMillis2 >= SLEEPMODE_TIME) && (DischargeState == 2)) { //Activate sleep mode
    ActivateSleep();
    PreviousMillis2 = millis();
  }

}
